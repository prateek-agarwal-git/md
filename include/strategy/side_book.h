#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <map>
#include <ostream>
#include <unordered_map>
#include <vector>

namespace strategy {

template <typename Comparator> struct SideBook {
  struct mbp_entry {
    // aggregated quantities on a given price level
    double price{};
    std::uint32_t quantity{};
    // num_orders is optional. Keeping it here for now. This field could well be
    // removed if we have written extensive tests and are confident of the
    // correctness of the implementation.
    std::uint32_t num_orders{};

    bool operator<(const mbp_entry &other) const {
      return Comparator()(price, other.price);
    }
    bool operator==(const mbp_entry &other) const {
      return common::almost_equal(price, other.price) &&
             quantity == other.quantity && num_orders == other.num_orders;
    }
  };
  inline std::ostream &operator()(std::ostream &os, const mbp_entry &e) {
    os << "price=" << e.price << "quantity=" << e.quantity
       << "num_orders=" << e.num_orders << std::endl;
    return os;
  }

  SideBook() { mbp_levels_.resize(5, {}); }
  mbp_entry get_top_level() const { return mbp_levels_[0]; }

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
    delete_from_mbp_levels(price, quantity);
  }
  void add_order(uint64_t order_id, double price, std::uint32_t quantity) {
    // it is assumed that the market data is clean and no order id will be
    // repeated for new order for a given side
    assert(!mbo_map_.contains(order_id));
    mbo_map_[order_id] = {.price = price, .quantity = quantity};
    // update price levels here. Do not add price level beyond the 5th level.
    for (int i = 0; i < 5; ++i) {
      if (common::almost_equal(price, mbp_levels_[i].price)) {
        mbp_levels_[i].quantity += quantity;
        mbp_levels_[i].num_orders += 1;
        break;
      }
      if (common::almost_equal(mbp_levels_[i].price, 0)) {
        mbp_levels_[i] = {
            .price = price, .quantity = quantity, .num_orders = 1};
        break;
      }
      if (Comparator()(price, mbp_levels_[i].price)) {
        // place of insertion;
        mbp_levels_.insert(
            mbp_levels_.begin() + i,
            mbp_entry{.price = price, .quantity = quantity, .num_orders = 1});
        mbp_levels_.resize(5);
        break;
      }
    }
  }

  void trade_order(uint64_t order_id, std::uint32_t quantity) {
    // if order_id is not present in the map, it is ignored. May be the feed is
    // present from a later point of time and the corresponding new order is
    // missed.
    auto it = mbo_map_.find(order_id);
    if (it == mbo_map_.end())
      return;
    auto &quantity_present_in_book = it->second.quantity;
    const auto book_price = it->second.price;

    if (quantity < quantity_present_in_book) {
      quantity_present_in_book -= quantity;
    } else {
      mbo_map_.erase(it);
    }
    delete_from_mbp_levels(book_price, quantity);
  }
  // useful for testing
  void reset() {
    mbo_map_.clear();
    mbp_levels_.resize(0);
    mbp_levels_.resize(5, {});
  }

private:
  void delete_from_mbp_levels(double price, uint32_t quantity) {
    for (auto &level : mbp_levels_) {
      if (common::almost_equal(price, level.price)) {
        level.quantity -= quantity;
        if (level.quantity == 0) {
          // clear the entry
          level.price = 0;
          level.num_orders = 0;
          // remove will just push the entries at the end and will move non zero
          // entries to the front.
          auto it =
              std::remove_if(mbp_levels_.begin(), mbp_levels_.end(),
                             [](const auto &mbp_entry) {
                               return common::almost_equal(mbp_entry.price, 0);
                             });
          std::for_each(it, mbp_levels_.end(), [](auto &item) {
            item = {};
          });
          break;
        }
      }
    }
  }
  struct mbo_entry {
    // price and quantity corresponding to a given order.
    double price{};
    std::uint32_t quantity{};
  };

  std::unordered_map<std::uint64_t, mbo_entry> mbo_map_;
  // Comparator cmp_;

  // this will contain the top five levels. Maintaining them in vector for
  // contiguous placement, better spatial locality and expected to be quicker
  // than a map based implementation.
  std::vector<mbp_entry> mbp_levels_;
  //  Hybrid book
  // first few levels in the vector for better cache locality.
  // Next orders in the std map ordered by price
  //  lower levels can be maintained in the map if we do not want to miss the
  //  lower levels. std::map<double, decltype(cmp_)> next_levels_;
};

} // namespace strategy