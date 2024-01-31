#pragma once
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <unistd.h>
#include <tuple>
#include <string>

namespace commandline {

static constexpr std::string_view usage = R"( 
Usage: ./<binary-name> --in_file=SCH.log --multicast_ip=239.50.50.12 --multicast_port=10812
)";
std::tuple<std::string, std::string, int16_t /*port*/>
parse_commandline(int argc, char **argv) {
  std::string in_file;
  std::string multicast_ip;
  int16_t multicast_port;
  // man 3 getopt
  while (true) {
    int option_index = 0;
    static struct option long_options[] = {
        {"in_file", required_argument, 0, 0},
        {"multicast_ip", required_argument, 0, 0},
        {"multicast_port", required_argument, 0, 0},
        {0, 0, 0, 0}};
    int c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 0:
      if (strcmp(long_options[option_index].name, "in_file") == 0) {
        in_file = optarg;
      } else if (strcmp(long_options[option_index].name, "multicast_ip") == 0) {
        multicast_ip = optarg;
      } else if (strcmp(long_options[option_index].name, "multicast_port") == 0) {
        multicast_port= std::atoi(optarg);
      }
      break;
    default:
      break;
    }
  }
  return {in_file, multicast_ip, multicast_port};
}

}