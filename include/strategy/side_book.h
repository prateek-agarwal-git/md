#pragma once
#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <map>

namespace strategy {
template <typename Comparator> struct SideBook {
  SideBook() { top_levels_.resize(5, {}); }
  void update_book(uint64_t order_id, double price, std::uint32_t quantity,
                   char order_category) {
    switch (order_category) {
    case 'N':
      add_order(order_id, price, quantity);
      break;
    case 'C':
      cancel_order(order_id);
      break;
    case 'T':
      // price that is present in the order book could be different from the
      // price on which order is traded. although in NSE, the orders are traded
      // on passive order present in the book.
      // quantity is sent as it could be a partial_trade
      trade_order(order_id, quantity);
      break;
    }
  }
  void cancel_order(uint64_t order_id) {
    auto it = mbo_map_.find(order_id);
    if (it == mbo_map_.end())
      return;
    auto price = it->second.price;
    auto quantity = it->second.quantity;
    mbo_map_.erase(it);
    // update price levels here
  }
  void add_order(uint64_t order_id, double price, std::uint32_t quantity) {
    // it is assumed that the market data is clean and no order id will be
    // repeated for new order for a given side
    assert(!mbo_map_.contains(order_id));
    mbo_map_[order_id] = {.price = price, .quantity = quantity};
    // update price levels here.
  }

  void trade_order(uint64_t order_id, double price, std::uint32_t quantity) {
    // if order_id is not present in the map, it is ignored. May be the feed is
    // present from a later point of time and the corresponding new order is
    // missed.
    auto it = mbo_map_.find(order_id);
    if (it == mbo_map_.end())
      return;
    auto &quantity_present_in_book = it->second.quantity;

    if (quantity < quantity_present_in_book) {
      quantity_present_in_book -= quantity;
    }
    // update price level with by reducing the traded quantity from the given
    // level.
  }

private:
  struct mbo_entry {
    double price{};
    std::uint32_t quantity{};
  };
  struct mbp_entry {
    double price{};
    std::uint32_t quantity{};
  };

  std::unordered_map<std::uint64_t, mbo_entry> mbo_map_;
  Comparator cmp_;
  //  Hybrid book
  // first few levels in the vector for better cache locality.
  // Next orders in the std map ordered by price
  std::vector<mbp_entry> top_levels_;
  std::map<double, decltype(cmp_)> next_levels_;
};

} // namespace strategy