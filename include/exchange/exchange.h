#pragma once
#include "common_defs.h"
#include <cctype>
#include <string>
#include <string_view>
#include <thread>

namespace exchange {
// templated on server type for ease of testing
template <typename Server> struct Exchange {
  Exchange( Server& server, std::ostream &os)
      : log_(os), server_(server) {}
  void start_reading() { server_.start_reading(); }
  void on_read(std::string_view payload) {
    auto request = reinterpret_cast<const common::Request *>(payload.data());
    char response[sizeof(common::Response) +
                  32]; // 32 is some arbitrary safety buffer. Remove later id
                       // possible.
    auto *response_ptr = reinterpret_cast<common::Response *>(response);
    response_ptr->order_id = request->order_id;
    response_ptr->side = tolower(request->side);
    response_ptr->order_category = tolower(request->order_category);
    response_ptr->price = request->price;
    response_ptr->quantity = request->quantity;
    log_<< *response_ptr;
    server_({response, sizeof(common::Response)});
  }
  void set_read_cb() {
    auto read_cb = [this](std::string_view request) { on_read(request); };
    server_.set_read_cb(read_cb);
  }
  void stop_reading() { server_.stop_reading(); }

  ~Exchange() { server_.stop_reading(); }

private:
  std::ostream &log_;
  Server& server_;
};
} // namespace exchange