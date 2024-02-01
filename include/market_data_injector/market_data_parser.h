#pragma once
#include "common_defs.h"
#include <functional>
#include <string_view>
#include <optional>
#include <ostream>

namespace MDI {
struct MarketDataParser {
  using WriterFn = std::function<void(std::string_view)>;
  MarketDataParser(WriterFn &&Fn,std::ostream& os) : os_(os),Fn_(Fn) {}
  void read_data(std::istream &);

private:
std::ostream& os_;
  std::optional<common::BinaryMarketData>
  parse_order(const std::string &order_line);
  WriterFn Fn_;
  char out_buffer_[1024]{};
};

} // namespace MDI