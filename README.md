

# Open Questions  / TODO

# To build Docker
docker-compose build

# To run containers
docker-compose up -d

# To unlock wallet (use your password)
docker exec -it blackjack_nodeosd_1 /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900 wallet unlock --password <pw>

# To view live logs
docker logs -f blackjack_nodeosd_1

# Setup the following aliases
alias cleos='docker exec -it eosjack_nodeosd_1 /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900'
alias eosiocpp='docker exec -it eosjack_nodeosd_1 eosiocpp'

# Here is the command to compile
eosiocpp -g /eosdev/blackjack/blackjack.abi /eosdev/blackjack/blackjack.hpp && eosiocpp -o /eosdev/blackjack/blackjack.wast /eosdev/blackjack/blackjack.cpp

