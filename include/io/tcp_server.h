#pragma once
#include "common_defs.h"
#include <arpa/inet.h>
#include <cstring>
#include <functional>
#include <ostream>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
namespace io {
struct TCPServer {
  using ReadCb = std::function<void(std::string_view)>;
  // writing callback registered wih exchange
  TCPServer(const std::string &address_info, std::ostream &os) : log_(os) {
    const auto [ip, port] = common::get_ip_port(address_info);
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
      log_ << "TCPServer: Socket Error. errno=" << errno << std::endl;
      exit(EXIT_FAILURE);
    }
    std::memset(&server_addr_, 0, sizeof(server_addr_));
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = port;
    server_addr_.sin_addr.s_addr = inet_addr(ip.c_str());

    if (bind(server_fd_, (struct sockaddr *)&server_addr_,
             sizeof(server_addr_)) < 0) {
      log_ << "TCPServer: Bind Error. errno=" << errno << std::endl;
      exit(EXIT_FAILURE);
    }
    if (listen(server_fd_, 5) < 0) {
      log_ << "TCPServer: Listen Error. errno=" << errno << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  void start_reading() {
    // only a single strategy will connect to this simple mock exchange.
    // Otherwise, epoll etc. or third party boost asio libraries can be used
    socklen_t addr_size = sizeof(client_addr_);
    client_fd_ =
        accept(server_fd_, (struct sockaddr *)&client_addr_, &addr_size);
    while (!stop_reading_) {
      std::memset(in_buffer_, 0, sizeof(in_buffer_));
      int count = recv(client_fd_, in_buffer_, sizeof(in_buffer_), 0);
      read_cb_({in_buffer_, std::size_t(count)});
      // the functor write callback will be called by the exchange on the same
      // thread.
    }
  }
  void stop_reading() { stop_reading_ = true; }
  void operator()(std::string_view data) {
    if (client_fd_ < 0) {
      log_ << "TCPServer: no request is received from the server. This should not be "
              "called."
           << std::endl;
      return;
    }
    int c =send(client_fd_, data.data(), data.size(), 0);

    assert(c == data.size());
    if (c < 0){
      log_ << "TCPServer: Send error. errno = " <<errno << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  ~TCPServer(){
    close(server_fd_);
    close(client_fd_);
  }

private:
  int server_fd_{-1};
  int client_fd_{-1};
  struct sockaddr_in server_addr_ {};

  struct sockaddr_in client_addr_ {};
  std::ostream &log_;
  char in_buffer_[128];
  ReadCb read_cb_;
  bool stop_reading_{false};
};

} // namespace io