#include "strategy/strategy.h"
#include "strategy/commandline_parser.h"
#include <fstream>
#include <iostream>
#include <string_view>
#include <thread>

static constexpr std::string_view usage =
    R"(Usage: ./<binary-name> --in_file=SCH.log --multicast_group_1=239.50.50.12:10812 --multicast_group_2=239.50.50.17:10817 --exchange_connection_1=127.0.0.1:8080 --exchange_connection_2=127.0.0.1:8081)";
int main(int argc, char **argv) {

  auto [multicast_group_1, multicast_group_2, exchange_connection_1,
        exchange_connection_2, log_file] =
      strategy::parse_commandline(argc, argv);
  if (multicast_group_1.empty() || exchange_connection_1.empty()) {
    std::cout << usage << std::endl;
    exit(EXIT_FAILURE);
  }
  if (log_file.empty()) {
    log_file = "exchange_" + std::to_string(getpid()) + ".txt";
  }
  std::ofstream ofs(log_file, std::ios::out);
  strategy::StrategyThread T1(ofs, exchange_connection_1, multicast_group_1);
  strategy::StrategyThread T2(ofs, exchange_connection_2, multicast_group_2);

  std::thread t1{&strategy::StrategyThread::start_reading, &T1};
  std::thread t2{&strategy::StrategyThread::start_reading, &T2};
  
  t1.join();
 t2.join();
  return 0;
}