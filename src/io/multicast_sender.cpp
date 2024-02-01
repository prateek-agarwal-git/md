#include "io/multicast_sender.h"
#include "common_defs.h"
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>

namespace io {
void MulticastSender::operator()(std::string_view data) {
  int cnt = sendto(fd_, data.data(), data.size(), 0, (struct sockaddr *)&addr_,
                   sizeof(addr_));
  if (cnt < 0) {
    os_ << "sendto Error. errno = " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
}

MulticastSender::MulticastSender(const std::string &address_info,
                                 std::ostream &os)
    : os_(os) {
  auto [group_ip, port] = common::get_ip_port(address_info);
  std::cout <<"group_ip MCS="  << group_ip<< std::endl;
  std::cout <<"port MCS="  << port<< std::endl;
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ < 0) {
    os_ << "socket opening error. errno=" << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  std::memset((char *)&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  addr_.sin_addr.s_addr = inet_addr(group_ip.c_str());
}
} // namespace io