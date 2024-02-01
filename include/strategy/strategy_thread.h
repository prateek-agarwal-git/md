#pragma once

#include "io/multicast_receiver.h"
#include "io/tcp_client.h"
#include "strategy/order_book.h"
#include "strategy/quoter.h"
#include "strategy/response_reader.h"
#include <iostream>
#include <ostream>

namespace strategy {
// this  module will listen on a single multicast channel and update the order
// book and make the quote if required.
struct StrategyThread {
  StrategyThread(std::ostream &log, const std::string &tcp_address_info,
                 const std::string &multicast_address_info)
      : log_(log), reader_(log), tcp_client_(tcp_address_info, log, reader_),
        quoter_(tcp_client_), book_(quoter_, log),
        receiver_(multicast_address_info, log, book_) {
    std::cout << "strategy thread" << std::endl;
  }
  void start_reading() {
    std::cout << "start reading strategy" << std::endl;
    receiver_.start_reading();
  }
  void stop_reading() { receiver_.stop_reading(); }

private:
  std::ostream &log_;
  ResponseReader reader_;
  io::TCPClient tcp_client_;
  Quoter quoter_;
  OrderBook book_;
  io::MulticastReceiver receiver_;
};
} // namespace strategy