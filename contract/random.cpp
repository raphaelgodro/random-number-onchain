#include <blackjack.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/currency.hpp>

#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/types.hpp>
#include <eosiolib/action.hpp>
#include <eosiolib/symbol.hpp>
#include <eosiolib/crypto.h>
#include <cstring>
#include <math.h>

using namespace std;
using namespace eosio;

void blackjack::stay(const account_name player)
{
    require_auth(player);
    auto g_t = game_table (_self, _self);
    auto g_itr = g_t.find (player);
    eosio_assert (g_itr != g_t.end(), "Player does not have an active game.");
    eosio_assert (g_itr->status == 0, "Game has ended.");

    const uint8_t player_score = getbestscore (g_itr->player_cards);
    uint8_t dealer_score = getbestscore (g_itr->dealer_cards);
    
    g_t.modify (g_itr, _self, [&](auto &g) {

        while ( (dealer_score <= 17 || 
                 dealer_score < player_score) &&
                dealer_score < 21 ) {
            
            print ("\nHand: -------------------------------\n");
            print ("Dealer Best Score   : ", to_string(dealer_score).c_str(), "\n");
            print ("Player Best Score   : ", to_string(player_score).c_str(), "\n");

            // pick dealer card
            uint8_t d_index       = (dealer_score + player + now())  % 12 ;
            uint8_t d_suit        = (dealer_score + player + now()) % 3;
            uint8_t d_card_value  = deck[d_index];

            g.dealer_indexes.push_back (d_index);
            g.dealer_cards.push_back (d_card_value);
            g.dealer_suits.push_back (d_suit);

            dealer_score = getbestscore (g.dealer_cards);
        }
    });

    if (dealer_score > 21) {
        endgame (player, true, false, false);
    } else if (dealer_score > player_score) {
        endgame (player, false, false, false);
    } else if (dealer_score == player_score) {
        endgame (player, false, false, true);
    } else if (player_score > dealer_score) {
        endgame (player, true, false, false);
    }
}

void blackjack::hit (const account_name player) {

    require_auth (player);
    auto g_t = game_table (_self, _self);
    auto g_itr = g_t.find (player);
    eosio_assert (g_itr != g_t.end(), "Player does not have an active game.");
    eosio_assert (g_itr->status == 0, "Game has ended.");

    vector<uint8_t> cards;
    g_t.modify (g_itr, _self, [&](auto &g) {

        // pick player card
        uint8_t p_index       = (player + now())  % 12 ;
        uint8_t p_suit        = (player + now()) % 4;
        uint8_t p_card_value  = deck[p_index];

        g.player_indexes.push_back (p_index);
        g.player_cards.push_back (p_card_value);
        g.player_suits.push_back (p_suit);

        cards = g.player_cards;
    });

    if (getbestscore(cards) > 21) {
        endgame (player, false, false, false);
    } else if (getbestscore (cards) == 21) {
        endgame (player, true, false, false);
    }
}

void blackjack::end(const account_name player) {
    
    require_auth (player);
    auto g_t = game_table (_self, _self);
    auto g_itr = g_t.find (player);
    eosio_assert (g_itr != g_t.end(), "Player does not have an active game.");

    g_t.erase (g_itr);
}

void blackjack::setconfig (const account_name token_contract, 
                            const asset minimum_bet, 
                            const asset maximum_bet,
                            const account_name bj_token_contract,
                            const string        bj_symbol_string,
                            const uint8_t       bj_precision) {

    require_auth (_self);
    auto c_t = config_table (_self, _self);
    auto c_itr = c_t.find(0);
    if (c_itr == c_t.end()) {
        c_t.emplace (_self, [&](auto &c) {
            c.token_contract = token_contract;
            c.minimum_bet = minimum_bet;
            c.maximum_bet = maximum_bet;
            c.bj_token_contract = bj_token_contract;
            c.bj_symbol = string_to_symbol (bj_precision, bj_symbol_string.c_str());
        });
    } else {
        c_t.modify (c_itr, _self, [&](auto &c) {
            c.token_contract = token_contract;
            c.minimum_bet = minimum_bet;
            c.maximum_bet = maximum_bet;
            c.bj_token_contract = bj_token_contract;
            c.bj_symbol = string_to_symbol (bj_precision, bj_symbol_string.c_str());
        });
    }
}


void blackjack::apply(const account_name contract, const account_name act)
{

    if (act == N(transfer))
    {
        transferReceived(unpack_action_data<currency::transfer>(), contract);
        return;
    }

    auto &thiscontract = *this;

    switch (act)
    {
        EOSIO_API(blackjack, (hit)(stay)(setconfig))
    };
}

void blackjack::transferReceived(const currency::transfer &transfer, const account_name code)
{
    if (transfer.to != _self)
    {
        return;
    }

    print("Account Name     :   ", name{code}, "\n");
    print("From             :   ", name{transfer.from}, "\n");
    print("To               :   ", name{transfer.to}, "\n");
    print("Asset            :   ", transfer.quantity, "\n");
    print("Received Amount  :   ", transfer.quantity.amount, "\n");
    print("Received Symbol  :   ", transfer.quantity.symbol, "\n");
    print("Memo             :   ", transfer.memo, "\n");

    // Create New Game
    game_table g_t (_self, _self);
    auto g_itr = g_t.find (transfer.from);
    if (g_itr != g_t.end()) {
        g_t.erase (g_itr);
    }

    config_table c_t (_self, _self);
    auto c_itr = c_t.begin();
    eosio_assert (c_itr != c_t.end(), "Config has not been set.");
    eosio_assert (c_itr->token_contract == code, "Token contract does not match config.");
    eosio_assert (transfer.quantity >= c_itr->minimum_bet, "Bet below minimum.");
    eosio_assert (transfer.quantity <= c_itr->maximum_bet, "Bet above maximum.");

    vector<uint8_t> cards;
    g_t.emplace (transfer.from, [&](auto &g) {
        g.player    = transfer.from;
        g.bet       = transfer.quantity;
        
        // pick player card #1
        uint8_t p_index_1         = now()  % 12 ;
        uint8_t p_suit_1          = now() % 3;
        uint8_t p_card_value_1    = deck[p_index_1];

        // pick player card #2
        uint8_t p_index_2         = (p_index_1 + now()) % 12;
        uint8_t p_suit_2          = (p_index_1 + now()) % 3;
        uint8_t p_card_value_2    = deck[p_index_2];

        // pick dealer card #1
        uint8_t d_index_1         = (p_index_2 + now()) % 12;
        uint8_t d_suit_1          = (p_index_2 + now()) % 3;
        uint8_t d_card_value_1    = deck[d_index_1];

        g.player_indexes.push_back (p_index_1);
        g.player_cards.push_back (p_card_value_1);
        g.player_suits.push_back (p_suit_1);

        g.player_indexes.push_back (p_index_2);
        g.player_cards.push_back (p_card_value_2);
        g.player_suits.push_back (p_suit_2);
        
        cards   = g.player_cards;
        
        g.dealer_indexes.push_back (d_index_1);
        g.dealer_cards.push_back (d_card_value_1);
        g.dealer_suits.push_back (d_suit_1);
    });

    if (getbestscore(cards) == 21) {
        endgame (transfer.from, true, true, false);
    } 
}

extern "C"
{
    //using namespace bay;
    using namespace eosio;

    void apply(uint64_t receiver, uint64_t code, uint64_t action)
    {
        auto self = receiver;
        blackjack contract(self);
        contract.apply(code, action);
        eosio_exit(0);
    }
}