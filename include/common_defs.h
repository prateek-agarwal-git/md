#pragma once
#include <cstdint>

namespace common {
enum class Protocol { TCP, UDP };
enum class Side : char { Bid = 'B', Sell = 'S' };
enum class OrderCategory: char{ New = 'N', Cancel = 'C', Trade = 'T'};

struct BinaryMarketData {
    static constexpr std::size_t symbol_length = 8;
    uint64_t epoch;
    uint64_t order_id;
    char symbol[symbol_length]{};// symbol will be null delimited.
    char side; // 'can be 'B'  or 'S'
    char order_category; //, 'N', 'C', 'T'
    double price;
    uint32_t quantity;
} __attribute__((packed));
} // namespace common