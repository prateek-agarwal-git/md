#include "io/multicast_sender.h"
#include "market_data_injector/commandline_parser.h"
#include "market_data_injector/market_data_parser.h"
#include <iostream>

static constexpr std::string_view usage =
    R"(Usage: ./<binary-name> --in_file=SCH.log --multicast_address=239.50.50.12:10812)";

int main(int argc, char **argv) {
  auto [in_file, multicast_address] = MDI::parse_commandline(argc, argv);
  if (in_file.empty() || multicast_address.empty()) {
    std::cout << usage << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "infile_name " << in_file << std::endl;
  std::cout << "multicast_address " << multicast_address << std::endl;
  return 0;
}