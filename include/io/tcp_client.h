#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <functional>
#include <ostream>
#include <string_view>
#include <unistd.h>
namespace io {
struct TCPClient {
  using ReadCb = std::function<void(std::string_view)>;
  TCPClient(const std::string &address_info, std::ostream &os,
            ReadCb &&read_cb);

  void operator()(std::string_view payload);

~TCPClient();
private:
  int fd_{-1};
  struct sockaddr_in addr_ {};
  std::ostream &log_;
  char in_buffer_[128];
  ReadCb read_cb_;
};
} // namespace io