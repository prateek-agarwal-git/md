#pragma once
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <string>
#include <tuple>
#include <unistd.h>

namespace strategy {

std::tuple<std::string, std::string, std::string, std::string, std::string>
parse_commandline(int argc, char **argv) {
  std::string multicast_group_1;
  std::string multicast_group_2;
  std::string exchange_connection_1;
  std::string exchange_connection_2;
  std::string log_file;
  // man 3 getopt
  while (true) {
    int option_index = 0;
    static struct option long_options[] = {
        {"multicast_group_1", required_argument, 0, 0},
        {"multicast_group_2", required_argument, 0, 0},
        {"exchange_connection_1", required_argument, 0, 0},
        {"exchange_connection_2", required_argument, 0, 0},
        {"log_file", required_argument, 0, 0},
        {0, 0, 0, 0}};
    int c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 0:
      if (strcmp(long_options[option_index].name, "multicast_group_1") == 0) {
        multicast_group_1 = optarg;
      } else if (strcmp(long_options[option_index].name, "multicast_group_2") ==
                 0) {
        multicast_group_2 = optarg;
      } else if (strcmp(long_options[option_index].name,
                        "exchange_connection_1") == 0) {
        exchange_connection_1 = optarg;
      } else if (strcmp(long_options[option_index].name,
                        "exchange_connection_2") == 0) {
        exchange_connection_2 = optarg;
      }else if (strcmp(long_options[option_index].name,
                        "log_file") == 0) {
        log_file= optarg;
      }
      break;
    default:
      break;
    }
  }
  return {multicast_group_1, multicast_group_2, exchange_connection_1,
          exchange_connection_2, log_file};
}

} // namespace strategy