#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <string>

namespace common {
enum class Side : char { Bid = 'B', Sell = 'S' };
enum class OrderCategory : char { New = 'N', Cancel = 'C', Trade = 'T' };

std::pair<std::string /*ip_octets*/,
          uint16_t /*port number*/> inline get_ip_port(const std::string
                                                           &address) {
  auto index = address.find_first_of(":");
  if (index == std::string::npos) {
    assert(false);
  }
  std::string ip = address.substr(0, index);
  std::uint16_t port = std::stoi(address.substr(index + 1));
  return {ip, port};
}
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

// each mock exchange is for one symbol
struct Request {
  uint64_t order_id;   // assume it to be a client order id filled by the client
                       // for simplicity of exchange
  char side;           // 'can be 'B'  or 'S'
  char order_category; // 'N', 'C'
  double price;
  uint32_t quantity;
} __attribute__((packed));

// exchange will echo the request by turning side and order category into lower
// case.
struct Response {
  uint64_t order_id;   // assume it to be a client order id filled by the client
                       // for simplicity of exchange
  char side;           // 'b'  or 's'
  char order_category; // 'n'or  'c'
  double price;
  uint32_t quantity;
} __attribute__((packed));
} // namespace common