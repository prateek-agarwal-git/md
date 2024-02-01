#!/usr/bin/env bash

./build/exchange 127.0.0.1:8080 ./build/exchange.txt &
sleep 10
./build/strategy --multicast_group_1=239.50.50.12:10812 --exchange_connection_1=127.0.0.1:8080 --log_file=./build/strategy.txt &
sleep 5 
./build/market_data_injector --in_file=./problem_statement/MarketData_SC/SCH.log --multicast_address=239.50.50.12:10812 --log_file=./build/mdi.txt 
