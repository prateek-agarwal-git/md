#pragma once
#include "common_defs.h"
#include <functional>
#include <string_view>

namespace tests {
struct TestServer {
  using ReadCb = std::function<void(std::string_view)>;
  TestServer(common::Request request) : request_(request) {}
  void start_reading() { read_cb_({reinterpret_cast<const char*>(&request_), sizeof(request_)});} 
  void stop_reading() {}
  void set_read_cb(ReadCb &&read_cb) { read_cb_ = read_cb; }
  void operator()(std::string_view data) {
    response_ = *reinterpret_cast<const common::Response *>(data.data());
  }
  auto response() const { return response_; }

private:
  ReadCb read_cb_;
  const common::Request request_;
  common::Response response_{};
};

} // namespace tests