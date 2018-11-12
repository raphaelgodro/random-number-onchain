#----------------------------
# VERSION 1.0.0
# AUTHOR: Raphael Gaudreault
# DESCRIPTION: base image of EOSRANDOMEXAMPLE

FROM eosio/eos-dev:v1.2.5


COPY ./entrypoint.sh /

RUN chmod +x ./entrypoint.sh