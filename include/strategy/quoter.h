#pragma once
#include "common_defs.h"
#include "double_comparators.h"
#include <cstdint>
#include <functional>
#include <string_view>

namespace strategy {
// computes signal and sends order if required based on the top levelprice and
// quantity of the book on the side on which order is received
struct Quoter {
  //   tcp client callback
  using WriterCb = std::function<void(std::string_view)>;
  Quoter(WriterCb &&cb) : write_cb_(cb) {}
  void operator()(double price, std::uint32_t quantity, char order_side) {
    // trading logic, if the price or quantity levels change at the top
    // level,fire and order on the new price with quantity 5.
    if (order_side == 'B') {
      if (!common::almost_equal(price, prev_bid_price_) ||
          !(quantity == prev_bid_qty_)) {
        common::Request r{.order_id = bid_curr_order_id_++,
                          .side = order_side,
                          .order_category = 'N',
                          .price = price,
                          .quantity = 5};
        write_cb_({reinterpret_cast<char *>(&r), sizeof(r)});
      }
      prev_bid_price_ = price;
      prev_bid_qty_ = quantity;
    } else {

      if (!common::almost_equal(price, prev_ask_price_) ||
          !(quantity == prev_ask_qty_)) {
        common::Request r{.order_id = ask_curr_order_id_++,
                          .side = order_side,
                          .order_category = 'N',
                          .price = price,
                          .quantity = 5};
        write_cb_({reinterpret_cast<char *>(&r), sizeof(r)});
      }

      prev_ask_price_ = price;
      prev_ask_qty_ = quantity;
    }
  }

private:
  WriterCb write_cb_;
  std::uint64_t bid_curr_order_id_{1};
  std::uint64_t ask_curr_order_id_{1};

  double prev_bid_price_{};
  std::uint32_t prev_bid_qty_{};

  double prev_ask_price_{};
  std::uint32_t prev_ask_qty_{};
};

} // namespace strategy