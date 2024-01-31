#pragma once
#include "side_book.h"
#include "common_defs.h"
#include <cstdint>
#include <functional>
#include <unordered_map>
namespace strategy {
struct OrderBook {
  OrderBook() {}
  void operator()(const common::BinaryMarketData& bmd);

private:
  SideBook<std::greater<double>> bid_book_;
  SideBook<std::less<double>> ask_book_;
};

} // namespace strategy