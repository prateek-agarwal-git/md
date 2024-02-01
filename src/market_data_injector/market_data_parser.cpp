#include "market_data_injector/market_data_parser.h"
#include "common_defs.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
namespace MDI {

void MarketDataParser::read_data(std::istream &is) {
  while (!is.eof()) {
    std::string line;
    std::getline(is, line, '\n');
    auto data = parse_order(line);
    if (!data)
      continue;
    auto bmd = *data;
    os_ <<bmd;
    std::memcpy(out_buffer_, reinterpret_cast<void *>(&bmd), sizeof(bmd));
    Fn_({out_buffer_, sizeof(bmd)});
    std::memset(out_buffer_, 0, sizeof(out_buffer_));
  }
}

std::optional<common::BinaryMarketData>
MarketDataParser::parse_order(const std::string &line) {
  // TODO:: clean this up with helper methods if time permits
  std::stringstream ss(line);
  common::BinaryMarketData data;
  uint64_t epoch;
  ss >> epoch;
  data.epoch = epoch;
  uint64_t order_id;
  ss >> order_id;
  data.order_id = order_id;
  std::string symbol;
  ss >> symbol;
  std::memcpy(data.symbol, symbol.c_str(), symbol.size());
  std::string side;
  ss >> side;
  if (side == "SELL") {
    data.side = 'S';
  } else if (side == "BUY") {
    data.side = 'B';
  } else {
    // some trades are marked unknown
    return {};
  }

  std::string order_category;
  ss >> order_category;
  if (order_category == "NEW") {
    data.order_category = 'N';
  } else if (order_category == "CANCEL") {
    data.order_category = 'C';
  } else if (order_category == "TRADE") {
    data.order_category = 'T';
  } else {
    return {};
  }
  double price;
  ss >> price;
  data.price = price;
  uint32_t quantity;
  ss >> quantity;
  data.quantity = quantity;
  return data;
}

} // namespace MDI