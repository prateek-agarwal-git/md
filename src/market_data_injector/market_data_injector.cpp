#include "market_data_injector/market_data_parser.h"
#include "market_data_injector/commandline_parser.h"
#include "io/multicast_sender.h"
#include <iostream>

static constexpr std::string_view usage = R"(Usage: ./<binary-name> --in_file=SCH.log --multicast_ip=239.50.50.12 --multicast_port=10812)";

int main(int argc, char ** argv){
  auto [in_file, multicast_ip, multicast_port ]=
      commandline::parse_commandline(argc, argv);
    if (in_file.empty() || multicast_ip.empty() || multicast_port == 0){
        std::cout << usage << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout <<"infile_name " << in_file <<std::endl;
    std::cout <<"multicast_ip " << multicast_ip<<std::endl;
    std::cout <<"multicast_port " << multicast_port<<std::endl;
    return 0;

}