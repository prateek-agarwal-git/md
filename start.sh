#!/usr/bin/env bash

./build/exchange 127.0.0.1:8080 ./build/exchange_1.txt &
sleep 2
./build/exchange 127.0.0.1:8081 ./build/exchange_2.txt &
sleep 10
./build/strategy --multicast_group_1=239.50.50.12:10812 --exchange_connection_1=127.0.0.1:8080 --multicast_group_2=239.50.50.17:10817 --exchange_connection_2=127.0.0.1:8081 --log_file=./build/strategy.txt &
sleep 5 
./build/market_data_injector --in_file=./problem_statement/MarketData_SC/SCS.log --multicast_address=239.50.50.12:10812 --log_file=./build/mdi_1.txt &
./build/market_data_injector --in_file=./problem_statement/MarketData_SC/SCH.log --multicast_address=239.50.50.17:10817 --log_file=./build/mdi_2.txt 
