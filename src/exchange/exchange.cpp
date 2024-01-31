#include "exchange/exchange.h"
#include "io/tcp_server.h"
#include <iostream>


int main(){
    exchange::Exchange<io::TCPServer> E("127.0.0.1:8080",std::cout );


    return 0;
}