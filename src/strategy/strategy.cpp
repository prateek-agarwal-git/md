#include "strategy/strategy.h"
#include "strategy/commandline_parser.h"
#include <iostream>
#include <string_view>

static constexpr std::string_view usage =
    R"(Usage: ./<binary-name> --in_file=SCH.log --multicast_group1=239.50.50.12:10812 --multicast_group2=239.50.50.12:10812 --exchange_connection1=127.0.0.1:8080 --exchange_connection2=127.0.0.1:8081)";
int main(int argc, char **argv) {

  auto [multicast_group_1, multicast_group_2, exchange_connection_1,
        exchange_connection_2, log_file] =
      strategy::parse_commandline(argc, argv);
  if (multicast_group_1.empty() || exchange_connection_1.empty()) {
    std::cout << usage << std::endl;
    exit(EXIT_FAILURE);
  }
  strategy::Strategy S({.multicast_group_1 = multicast_group_1,
                        .exchange_connection_1 = exchange_connection_1,
                        .multicast_group_2 = multicast_group_2,
                        .exchange_connection_2 = exchange_connection_2,
                        .log = std::cout});
  S.start_reading();
  return 0;
}