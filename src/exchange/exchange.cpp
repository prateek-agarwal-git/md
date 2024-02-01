#include "exchange/exchange.h"
#include "io/tcp_server.h"
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <unistd.h>

static constexpr std::string_view usage =
    R"(./<binary-name> 127.0.0.1:10100 <logfile_path>)";

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << usage << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string log_file;
  if (argc > 2) {
    log_file = argv[2];
  } else {
    log_file = "/tmp/exchange_" + std::to_string(getpid()) + ".txt";
  }
  std::ofstream ofs(log_file, std::ios::out);
  io::TCPServer S(std::string(argv[1]), ofs);
  exchange::Exchange E(S, ofs);
  E.set_read_cb();
  // TODO write signalhandler for stopping the exchange thread.
  // std::thread t{&exchange::Exchange<io::TCPServer>::stop_reading, &E };
  E.start_reading();
  ofs.close();
  // t.join();
  return 0;
}