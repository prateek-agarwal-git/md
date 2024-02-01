#pragma once
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <functional>
#include <ostream>
#include <string>
#include <string_view>

namespace io {
struct MulticastReceiver {

  using ReaderFn = std::function<void(std::string_view)>;

  MulticastReceiver(const std::string &address_info,
                    std::ostream &os, ReaderFn &&fn);

  void start_reading();
  void stop_reading();
private:
  int fd_;
  std::ostream &os_;
  struct sockaddr_in addr_;
  ReaderFn fn_;
  bool stop_reading_{false};
  char in_buffer_[128];
};

} // namespace io