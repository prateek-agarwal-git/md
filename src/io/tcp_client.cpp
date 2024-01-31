#include "io/tcp_client.h"
#include "common_defs.h"

namespace io {

TCPClient::TCPClient(const std::string &address_info, std::ostream &os,
                     ReadCb &&read_cb)
    : log_(os), read_cb_(read_cb) {
  const auto [ip, port] = common::get_ip_port(address_info);
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ < 0) {
    log_ << "TCPC: Socket Error. errno = " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  std::memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = port;
  addr_.sin_addr.s_addr = inet_addr(ip.c_str());
  if (connect(fd_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0) {
    log_ << "TCPC: Connection Error. errno = " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  log_ << "TCPC: Successfully connected to the server. " << std::endl;
}

void TCPClient::operator()(std::string_view payload) {
  int status = send(fd_, payload.data(), payload.size(), 0);
  if (status < 0) [[unlikely]] {
    log_ << "TCPC: Send error. errno = " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  std::memset(in_buffer_, 0, sizeof(in_buffer_));
  int count = recv(fd_, in_buffer_, sizeof(in_buffer_), 0);
  if (count < 0) [[unlikely]] {
    log_ << "TCPC: Receive error. errno = " << errno << std::endl;
  }
  read_cb_({in_buffer_, std::size_t(count)});
}
TCPClient::~TCPClient(){
    close(fd_);
}
} // namespace io