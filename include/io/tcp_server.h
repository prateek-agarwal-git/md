#pragma once
#include "common_defs.h"
#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <functional>
#include <ostream>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>
namespace io {
struct TCPServer {
  using ReadCb = std::function<void(std::string_view)>;

  TCPServer(const std::string &address_info, std::ostream &os);
  void start_reading();
  void stop_reading();
  void set_read_cb(ReadCb &&read_cb) { read_cb_ = read_cb; }

  // writing callback registered wih exchange. called on the same thread that
  // receives the reading callback.
  void operator()(std::string_view data);
  ~TCPServer();

private:
  int server_fd_{-1};
  int client_fd_{-1};
  std::ostream &log_;
  char in_buffer_[128];
  ReadCb read_cb_;
  bool stop_reading_{false};
  const std::string address_info_;
};

} // namespace io