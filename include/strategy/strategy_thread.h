#pragma once

#include "io/multicast_receiver.h"
#include "io/tcp_client.h"
#include "strategy/order_book.h"
#include "strategy/quoter.h"
#include "strategy/response_reader.h"
#include <functional>
#include <iostream>
#include <ostream>
#include <string_view>

namespace strategy {
struct StrategyThread {
  StrategyThread(std::ostream &log, const std::string &tcp_address_info,
                 const std::string &multicast_address_info)
      : log_(log), reader_(log), tcp_client_(tcp_address_info, log, reader_),
        quoter_(tcp_client_), book_(quoter_, log),
        receiver_(multicast_address_info, log, book_) {}
  void start_reading() {
    receiver_.start_reading();
  }
  void stop_reading() { receiver_.stop_reading(); }

private:
  std::ostream &log_;
  ResponseReader reader_;
  io::TCPClient<ResponseReader> tcp_client_;
  Quoter<io::TCPClient<ResponseReader>> quoter_;
  OrderBook<strategy::Quoter<io::TCPClient<ResponseReader>>> book_;
  io::MulticastReceiver receiver_;
};
} // namespace strategy