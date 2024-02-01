#include "io/tcp_server.h"

namespace io {
TCPServer::TCPServer(const std::string &address_info, std::ostream &os)
    : log_(os), address_info_(address_info) {
  const auto [ip, port] = common::get_ip_port(address_info_);
  server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd_ < 0) {
    log_ << "TCPServer: Socket Error. errno=" << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  const int enable = 1;
  if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) <
      0) {
    log_ << "TCPServer: setsock opt error. errno=" << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in server_addr {};
  std::memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

  if (bind(server_fd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    log_ << "TCPServer: Bind Error. errno=" << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd_, 5) < 0) {
    log_ << "TCPServer: Listen Error. errno=" << errno << std::endl;
    exit(EXIT_FAILURE);
  }
}
void TCPServer::start_reading() {
  // only a single strategy will connect to this simple mock exchange.
  // Otherwise, epoll etc. or third party boost asio libraries can be used
  struct sockaddr_in client_addr {};
  socklen_t addr_size = sizeof(client_addr);
  client_fd_ = accept(server_fd_, (struct sockaddr *)&client_addr, &addr_size);
  if (client_fd_ < 0) {
    log_ << "TCPS: accept error. errno=" << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  while (!stop_reading_) {
    std::memset(in_buffer_, 0, sizeof(in_buffer_));
    int count = recv(client_fd_, in_buffer_, sizeof(in_buffer_), MSG_DONTWAIT);
    if (count == 0)
      break;
    if (count < 0)
      continue;
    read_cb_({in_buffer_, std::size_t(count)});
    // the functor write callback will be called by the exchange on the same
    // thread.
  }
  close(server_fd_);
  close(client_fd_);
}

void TCPServer::stop_reading() { stop_reading_ = true; }

void TCPServer::operator()(std::string_view data) {
  if (client_fd_ < 0) {
    log_ << "TCPServer: no request is received from the server. This should "
            "not be "
            "called."
         << std::endl;
    return;
  }

  int c = send(client_fd_, data.data(), data.size(), 0);
  assert(std::size_t(c) == data.size());
  if (c < 0) {
    log_ << "TCPServer: Send error. errno = " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
}

TCPServer::~TCPServer() { close(client_fd_); }
} // namespace io