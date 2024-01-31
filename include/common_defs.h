#pragma once
#include <cstdint>
#include <ostream>
#include <string>
#include <cstring>

namespace common {
enum class Side : char { Bid = 'B', Sell = 'S' };
enum class OrderCategory : char { New = 'N', Cancel = 'C', Trade = 'T' };

struct BinaryMarketData {
  static constexpr std::size_t symbol_length = 8;
  static constexpr double double_precision =
      0.001; // data in log files is upto 2 decimal places,this is kept
             // arbitrarily accordingly as there could be precision error while
             // reading log files.
  uint64_t epoch;
  uint64_t order_id;
  char symbol[symbol_length]{}; // All bytes will be null except the symbol.
  char side;                    // 'can be 'B'  or 'S'
  char order_category;          //, 'N', 'C', 'T'
  double price;
  uint32_t quantity;
  bool operator==(const BinaryMarketData &other) const {
    return epoch == other.epoch && order_id == other.order_id &&
           std::memcmp(symbol, other.symbol, symbol_length) == 0 &&
           side == other.side && order_category == other.order_category &&
           quantity == other.quantity &&
           std::abs(price - other.price) < double_precision;
  }
} __attribute__((packed));

inline std::ostream &operator<<(std::ostream &os,
                                const BinaryMarketData &market_data) {
  os << "BinaryMarketData: ";
  os << "epoch=" << market_data.epoch;
  os << ",order_id=" << market_data.order_id;
  os << ",symbol=" << std::string(market_data.symbol);
  os << ",side=" << market_data.side;
  os << ",order_category=" << market_data.order_category;
  os << ",price=" << market_data.price;
  os << ",quantity=" << market_data.quantity;
  os << std::endl;
  return os;
}
} // namespace common