.PHONY: all 
CC=g++
INC_DIR=./include/
CPPFLAGS=-g -std=c++20  -Wall -I$(INC_DIR)
DATA_INJ_OBJ_FILES=build/market_data_parser.o build/multicast_sender.o
STRATEGY_OBJ_FILES=build/multicast_receiver.o 
EXCHANGE_OBJ_FILES=build/tcp_server.o
OBJ_FILES=$(DATA_INJ_OBJ_FILES) $(STRATEGY_OBJ_FILES) $(EXCHANGE_OBJ_FILES) 
all: build/market_data_injector  build/tests build/strategy build/exchange build/tests
#ll: build/market_data_injector   build/strategy build/exchange


build/exchange: src/exchange/exchange.cpp ${INC_DIR}/* ${INC_DIR}/*/* $(EXCHANGE_OBJ_FILES)
	$(CC) $(CPPFLAGS)   src/exchange/exchange.cpp $(EXCHANGE_OBJ_FILES)  -o build/exchange

build/strategy: src/strategy/strategy.cpp ${INC_DIR}/* ${INC_DIR}/*/* $(STRATEGY_OBJ_FILES)
	$(CC) $(CPPFLAGS)   src/strategy/strategy.cpp $(STRATEGY_OBJ_FILES)  -o build/strategy

build/market_data_injector: src/market_data_injector/market_data_injector.cpp ${INC_DIR}/market_data_injector/*   $(DATA_INJ_OBJ_FILES)
	$(CC) $(CPPFLAGS)   src/market_data_injector/market_data_injector.cpp $(DATA_INJ_OBJ_FILES)  -o build/market_data_injector

build/market_data_parser.o: src/market_data_injector/market_data_parser.cpp ${INC_DIR}/* ${INC_DIR}/*/*
	$(CC) $(CPPFLAGS) -c src/market_data_injector/market_data_parser.cpp -o build/market_data_parser.o

build/tests: tests/tests.cpp ${INC_DIR}/* ${INC_DIR}/*/* $(OBJ_FILES)
	$(CC) $(CPPFLAGS)  tests/tests.cpp $(OBJ_FILES) -o build/tests

build/multicast_sender.o: src/io/multicast_sender.cpp ${INC_DIR}/* ${INC_DIR}/io/*
	$(CC) $(CPPFLAGS)  -c src/io/multicast_sender.cpp  -o build/multicast_sender.o 

build/multicast_receiver.o: src/io/multicast_receiver.cpp ${INC_DIR}/* ${INC_DIR}/io/*
	$(CC) $(CPPFLAGS)  -c src/io/multicast_receiver.cpp  -o build/multicast_receiver.o 


build/tcp_server.o: src/io/tcp_server.cpp ${INC_DIR}/* ${INC_DIR}/io/*
	$(CC) $(CPPFLAGS)  -c src/io/tcp_server.cpp  -o build/tcp_server.o 

clean:
	 rm -f  build/*
 
