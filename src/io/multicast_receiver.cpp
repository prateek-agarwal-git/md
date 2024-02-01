#include "io/multicast_receiver.h"
#include "common_defs.h"
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace io {
MulticastReceiver::MulticastReceiver(const std::string &address_info,
                                     std::ostream &os, ReaderFn &&fn)
    : os_(os), fn_(fn) {
  auto [group_ip, port] = common::get_ip_port(address_info);
  //std::cout << group_ip << std::endl;
  //std::cout << port << std::endl;

  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ < 0) {
    os_ << "MCR: socket opening error. errno=" << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  std::memset((char *)&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  addr_.sin_port = htons(port);

  if (bind(fd_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0) {
    os_ << "MCR: Bind error. errno= " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(group_ip.c_str());
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(fd_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    os_ << "MCR:setsockopt error. errno= " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  os_<< "multicast_listener created fd_" << fd_ << std::endl;
}

void MulticastReceiver::start_reading() {
  unsigned int addrlen = sizeof(addr_);

  while (!stop_reading_) {
    int cnt = recvfrom(fd_, in_buffer_, sizeof(in_buffer_), MSG_DONTWAIT,
                       (struct sockaddr *)&addr_, &addrlen);

    if (cnt == 0) {
      break;
    }

    if (cnt > 0) {
      fn_({in_buffer_, std::size_t(cnt)});
      std::memset(in_buffer_, 0, sizeof(in_buffer_));
    }
  }
}
void MulticastReceiver::stop_reading() { stop_reading_ = true; }
} // namespace io