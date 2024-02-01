#pragma once
#include "common_defs.h"
#include "double_comparators.h"
#include "side_book.h"
#include <cstdint>
#include <functional>
#include <string_view>
#include <unordered_map>
namespace strategy {
struct OrderBook {

  struct top_levels {
    SideBook<common::GreaterDouble>::mbp_entry bid_level;
    SideBook<common::LesserDouble>::mbp_entry ask_level;
  };
  using QuoterCb = std::function<void(double price, std::uint32_t quantity,
                                      char order_side)>;
  OrderBook(QuoterCb &cb) : quoter_cb_(cb) {}
  void operator()(std::string_view data) {
    auto *bmd = reinterpret_cast<const common::BinaryMarketData *>(data.data());
    if (bmd->side == 'S') {
      ask_book_.update_book(bmd->order_id, bmd->price, bmd->quantity,
                            bmd->order_category);
      const auto bid_level = get_top_levels().bid_level;
      quoter_cb_(bid_level.price, bid_level.quantity, bmd->side);
    } else {
      bid_book_.update_book(bmd->order_id, bmd->price, bmd->quantity,
                            bmd->order_category);
      const auto ask_level = get_top_levels().ask_level;
      quoter_cb_(ask_level.price, ask_level.quantity, bmd->side);
    }
  }
  top_levels get_top_levels() {
    return {bid_book_.get_top_level(), ask_book_.get_top_level()};
  }

private:
  QuoterCb &quoter_cb_;
  SideBook<common::GreaterDouble> bid_book_{};
  SideBook<common::LesserDouble> ask_book_{};
};

} // namespace strategy