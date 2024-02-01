#include "market_data_injector/market_data_injector.h"
#include "io/multicast_sender.h"
#include "market_data_injector/commandline_parser.h"
#include <fstream>
#include <iostream>

static constexpr std::string_view usage =
    R"(Usage: ./<binary-name> --in_file=SCH.log --multicast_address=239.50.50.12:10812 --log_file=<log_name>)";

int main(int argc, char **argv) {
  auto [in_file, multicast_address, log_file] =
      MDI::parse_commandline(argc, argv);
  if (in_file.empty() || multicast_address.empty()) {
    std::cout << usage << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ofstream ofs(log_file, std::ios::out);
  MDI::MarketDataInjector mdi(multicast_address, ofs);
  std::ifstream ifs(in_file, std::ios::in);
  mdi.read_data(ifs);
  sleep(5);
  ofs.close();
  ifs.close();
  return 0;
}