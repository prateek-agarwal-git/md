#pragma once
#include <iostream>
#include <string>

namespace strategy {
struct Strategy {
  struct strategy_args {
    std::string multicast_group_1;
    std::string multicast_group_2;
    std::string exchange_connection_1;
    std::string exchange_connection_2;
    std::ostream &log;
  };
  Strategy(strategy_args args):  log_(args.log){}
  

private:
  std::ostream &log_;
};

} // namespace strategy