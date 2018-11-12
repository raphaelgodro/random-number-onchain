cleos create account eosio blackjack EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio player EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio token EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio bjtoken EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn

cleos push action eosio updateauth '{"account":"blackjack","permission":"active","parent":"owner","auth":{"keys":[{"key":"EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn", "weight":1}],"threshold":1,"accounts":[{"permission":{"actor":"blackjack","permission":"eosio.code"},"weight":1}],"waits":[]}}' -p blackjack

eosiocpp -g /eosdev/blackjack/blackjack.abi /eosdev/blackjack/blackjack.hpp && eosiocpp -o /eosdev/blackjack/blackjack.wast /eosdev/blackjack/blackjack.cpp
eosiocpp -g /eosdev/mintabletoken/mintabletoken.abi /eosdev/mintabletoken/mintabletoken.hpp && eosiocpp -o /eosdev/mintabletoken/mintabletoken.wast /eosdev/mintabletoken/mintabletoken.cpp

cleos set contract blackjack /eosdev/blackjack
cleos set contract token /eosdev/eosio.token
cleos set contract bjtoken /eosdev/mintabletoken

cleos push action bjtoken create '["blackjack", "BJ", 0]' -p bjtoken
#cleos push action bjtoken issue '["player", "1 BJ", "Have a BJ!"]' -p blackjack

cleos push action token create '["token", "1000000.0000 EOS"]' -p token
cleos push action token issue '["player", "100000.0000 EOS", "memo"]' -p token
cleos push action token issue '["token", "1000.0000 EOS", "memo"]' -p token

cleos push action blackjack setconfig '["token", "100.0000 EOS", "200.0000 EOS", "bjtoken", "BJ", 0]' -p blackjack
cleos push action token issue '["blackjack", "200.0000 EOS", "memo"]' -p token

cleos push action token transfer '["player", "blackjack", "100.0000 EOS", "new game"]' -p player

cleos push action blackjack hit '["player"]' -p player
cleos get table blackjack blackjack games

cleos push action blackjack hit '["player"]' -p player
cleos get table blackjack blackjack games

cleos push action blackjack hit '["player"]' -p player
cleos get table blackjack blackjack games

cleos push action blackjack stay '["player"]' -p player
cleos get table blackjack blackjack games

cleos push action blackjack end '["player"]' -p player




cleos set contract token2 /eosdev/eosio.token
cleos push action token2 create '["token2", "1000000.0000 EOS"]' -p token2
cleos push action token2 issue '["player", "1000.0000 EOS", "memo"]' -p token2

