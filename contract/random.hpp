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
#include <functional>

using namespace eosio;
using namespace std;
using std::string;

class blackjack : public contract
{

  public:
    blackjack(account_name self) : contract(self) {}

    // @abi action
    void stay (const account_name player);

    // @abi action
    void hit(const account_name player);

    // @abi action
    void end(const account_name player);

    // @abi action 
    void setconfig (const account_name      token_contract, 
                    const asset             minimum_bet, 
                    const asset             maximum_bet,
                    const account_name      bj_token_contract,
                    const string            bj_symbol_string,
                    const uint8_t           bj_precision) ;

    void apply(const account_name contract, const account_name act);

    void transferReceived(const currency::transfer &transfer, const account_name code);

  private:

    const uint8_t   ENDED = 1;
    const uint8_t   deck[13]    = {1,2,3,4,5,6,7,8,9,10,10,10,10};

    // @abi table configs
    struct config {
        uint64_t        config_id;
        account_name    token_contract;
        asset           minimum_bet;
        asset           maximum_bet;
        account_name    bj_token_contract;
        symbol_type     bj_symbol;

        uint64_t        primary_key () const { return config_id; }
    };

    typedef eosio::multi_index<N(configs), config> config_table;

    // @abi table games
    struct game
    {
        account_name    player;
        asset           bet;
        
        // player hand
        vector<uint8_t> player_cards;
        vector<uint8_t> player_suits;
        vector<uint8_t> player_indexes;

        // dealer hand
        vector<uint8_t> dealer_cards;
        vector<uint8_t> dealer_suits;
        vector<uint8_t> dealer_indexes;

        uint8_t         status;

        account_name primary_key() const { return player; }
   };

    typedef eosio::multi_index<N(games), game> game_table;

    void paytoken(  const account_name  token_contract,
                    const account_name from,
                    const account_name to,
                    const asset token_amount,
                    const string memo)
    {

        config_table config(_self, _self);
        auto itr = config.begin();
        eosio_assert(itr != config.end(), "token contract is not set");

        print("---------- Transfer -----------\n");
        print("Token Contract   : ", name{token_contract}, "\n");
        print("From             : ", name{from}, "\n");
        print("To               : ", name{to}, "\n");
        print("Amount           : ", token_amount, "\n");
        print("Memo             : ", memo, "\n");

        action(
            permission_level{from, N(active)},
            token_contract, N(transfer),
            std::make_tuple(from, to, token_amount, memo))
            .send();

        print("---------- End Transfer -------\n");
    }

    void sendbj (   account_name    player ) {
        config_table config(_self, _self);
        auto c_itr = config.begin();
        eosio_assert(c_itr != config.end(), "Configuration is not set");

        asset token_amount = asset {1, c_itr->bj_symbol};
        string have_a_bj = string {"Have a BJ!"};
                
        action(
            permission_level{_self, N(active)},
            c_itr->bj_token_contract, N(issue),
            std::make_tuple(player, token_amount, have_a_bj))
            .send();
    }

    void endgame (account_name player, bool winner, bool blackjack, bool push) {
        auto g_t = game_table (_self, _self);
        auto g_itr = g_t.find (player);
        eosio_assert (g_itr != g_t.end(), "Player does not have an active game.");

        config_table config(_self, _self);
        auto c_itr = config.begin();
        eosio_assert(c_itr != config.end(), "Configuration is not yet.");

        if (push) {
            paytoken (c_itr->token_contract, _self, player, g_itr->bet, "Push" );
        } else if (winner && blackjack) {
            paytoken (c_itr->token_contract, _self, player, g_itr->bet * 250 / 100, "Blackjack!" );
        } else if (winner) {
            paytoken (c_itr->token_contract, _self, player, g_itr->bet * 2, "Winner!" );
        } else {
           sendbj (player);
        }

        g_t.modify (g_itr, player, [&](auto &g) {
            g.status = ENDED;
        });
    }
    
    uint8_t getbestscore (vector<uint8_t> cards) {

        uint8_t points = 0;
        bool aces = false;
        for(std::vector<uint8_t>::iterator it = cards.begin(); it != cards.end(); ++it) {

            if (*it == 1) {
                aces = true;
            }
            points += *it;    
        }

        if (aces && points+10 <= 21) {
            points += 10;
        }

        print ("Best Score:     ", to_string(points).c_str(), "\n");
        
        return points;
    }

    uint64_t hashacct(const string s)
    {
        std::hash<std::string> hash_fn;
        size_t hash = hash_fn(s);
        return hash;
    }


};
