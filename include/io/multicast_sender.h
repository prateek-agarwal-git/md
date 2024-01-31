#pragma once
#include <cstdint>
#include <cstring>
#include <ostream>
#include <string>
#include <string_view>
#include <arpa/inet.h>

namespace io {
struct MulticastSender {
  MulticastSender(const std::string &group_ip, std::uint16_t port,
                  std::ostream &os);
  int operator()(std::string_view data);
private:
  int fd_;
  std::ostream &os_;
  struct sockaddr_in addr_;
};
} // namespace io
