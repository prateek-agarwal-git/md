#pragma once
#include "common_defs.h"
#include <functional>
#include <string_view>

namespace MDI {
struct MarketDataParser {
  using WriterFn = std::function<void(std::string_view)>;
  MarketDataParser(WriterFn&& Fn) : Fn_(Fn) {}
  void read_data(std::istream& );

private:
  common::BinaryMarketData parse_order(const std::string &order_line);
  WriterFn Fn_;
  char out_buffer_[1024]{};
};

} // namespace MDI