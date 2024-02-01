#pragma once
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <string>
#include <tuple>
#include <unistd.h>

namespace MDI {

std::tuple<std::string, std::string, std::string> parse_commandline(int argc, char **argv) {
  std::string in_file;
  std::string multicast_address;
  std::string log_file{"/tmp/mdi_log" + std::to_string(getpid()) + ".txt"};
  // man 3 getopt
  while (true) {
    int option_index = 0;
    static struct option long_options[] = {
        {"in_file", required_argument, 0, 0},
        {"multicast_address", required_argument, 0, 0},
        {"log_file", required_argument, 0, 0},
        {0, 0, 0, 0}};
    int c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 0:
      if (strcmp(long_options[option_index].name, "in_file") == 0) {
        in_file = optarg;
      } else if (strcmp(long_options[option_index].name, "multicast_address") ==
                 0) {
        multicast_address = optarg;
      } else if (strcmp(long_options[option_index].name, "log_file") == 0) {
        log_file = optarg;
      }
      break;
    default:
      break;
    }
  }
  return {in_file, multicast_address,log_file};
}

} // namespace MDI