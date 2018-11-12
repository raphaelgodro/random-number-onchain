#!/bin/bash
################################################################################
# Author : Raphael Gaudreault
################################################################################

alias cleos='docker-compose exec keosd /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900'

alias cleos='docker-compose exec keosd /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900'
PUBKEY="EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn"
PRIVKEY="5JhhMGNPsuU42XXjZ57FcDKvbb7KLrehN65tdTQFrH51uruZLHi"

cleos wallet create --to-console
cleos wallet import  --private-key "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"
cleos wallet import --private-key $PRIVKEY

cleos create account eosio jack $PUBKEY $PUBKEY
cleos create account eosio player $PUBKEY $PUBKEY
cleos create account eosio token $PUBKEY $PUBKEY


cleos push action eosio updateauth '{"account":"jack","permission":"active","parent":"owner","auth":{"keys":[{"key":"EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn", "weight":1}],"threshold":1,"accounts":[{"permission":{"actor":"jack","permission":"eosio.code"},"weight":1}],"waits":[]}}' -p jack


eosiocpp -g /eosdev/blackjack/blackjack.abi /eosdev/blackjack/blackjack.hpp && eosiocpp -o /eosdev/blackjack/blackjack.wast /eosdev/blackjack/blackjack.cpp

cleos set contract jack /eosdev/blackjack/
cleos set contract token /eosdev/eosio.token


cleos push action token create '["token", "1000000.0000 EOS"]' -p token
cleos push action token issue '["player", "1000.0000 EOS", "memo"]' -p token
cleos push action token issue '["jack", "1000.0000 EOS", "memo"]' -p token
cleos push action token issue '["token", "1000.0000 EOS", "memo"]' -p token

cleos push action jack setconfig '["100.0000 EOS", "200.0000 EOS"]' -p jack

cleos push action token issue '["jack", "200.0000 EOS", "memo"]' -p token

cleos push action token transfer '["player", "jack", "100.0000 EOS", "new game"]' -p player

cleos push action jack hit '["player"]' -p player
cleos get table jack jack games

cleos push action jack hit '["player"]' -p player
cleos get table jack jack games

cleos push action jack hit '["player"]' -p player
cleos get table jack jack games

cleos push action jack stay '["player"]' -p player
cleos get table jack jack games

cleos push action jack end '["player"]' -p player