#pragma once
#include "strategy_thread.h"
#include <iostream>
#include <string>

namespace strategy {
struct Strategy {
  struct strategy_args {
    std::string multicast_group_1;
    std::string exchange_connection_1;
    std::string multicast_group_2;
    std::string exchange_connection_2;
    std::ostream &log;
  };

  Strategy(strategy_args args)
      : S1(args.log, args.exchange_connection_1, args.multicast_group_1),
        log_(args.log) {}
  void start_reading() { S1.start_reading(); }

private:
  StrategyThread S1;
  //  StrategyThread S2;

  std::ostream &log_;
};

} // namespace strategy