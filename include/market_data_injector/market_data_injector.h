#pragma once
#include "io/multicast_sender.h"
#include "market_data_injector/market_data_parser.h"
#include <iostream>

namespace MDI {

struct MarketDataInjector {
  MarketDataInjector(const std::string &address_info, std::ostream &log)
      : log_(log), ms_(address_info, log), mdp_(ms_, log) {}
  void read_data(std::istream &is) { mdp_.read_data(is); }

private:
  std::ostream &log_;
  io::MulticastSender ms_;
  MarketDataParser mdp_;
};
} // namespace MDI