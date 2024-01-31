.PHONY: all 
CC=g++
INC_DIR=./include/
CPPFLAGS=-g -std=c++20  -Wall -I$(INC_DIR)
OBJ_FILES=build/market_data_parser.o
DATA_INJ_OBJ_FILES=build/market_data_parser.o build/multicast_sender.o
all: build/market_data_injector  build/tests
#build/market_data_parser: src/main.cpp ${INC_DIR}/* ${INC_DIR}/*/* $(OBJ_FILES)
#	$(CC) $(CPPFLAGS)  src/main.cpp $(OBJ_FILES) -o build/market_data_parser -lpcap
#
# current directory path given as compile time definition for pcap tests
build/market_data_injector: src/market_data_injector/market_data_injector.cpp ${INC_DIR}/market_data_injector/*   $(DATA_INJ_OBJ_FILES)
	$(CC) $(CPPFLAGS)   src/market_data_injector/market_data_injector.cpp $(DATA_INJ_OBJ_FILES)  -o build/market_data_injector

build/market_data_parser.o: src/market_data_injector/market_data_parser.cpp ${INC_DIR}/* ${INC_DIR}/*/*
	$(CC) $(CPPFLAGS) -c src/market_data_injector/market_data_parser.cpp -o build/market_data_parser.o

# current directory path given as compile time definition for pcap tests
build/tests: tests/tests.cpp ${INC_DIR}/* ${INC_DIR}/*/* $(OBJ_FILES)
	$(CC) $(CPPFLAGS)  tests/tests.cpp $(OBJ_FILES) -o build/tests -DCURDIR=\"${CURDIR}\"

build/multicast_sender.o: src/io/multicast_sender.cpp ${INC_DIR}/* ${INC_DIR}/*/*
	$(CC) $(CPPFLAGS)  -c src/io/multicast_sender.cpp  -o build/multicast_sender.o 
#
clean:
	 rm -f  build/*
 
