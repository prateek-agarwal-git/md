#include "io/multicast_receiver.h"
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace io {
MulticastReceiver::MulticastReceiver(const std::string &group_ip,
                                     std::uint16_t port, std::ostream &os,
                                     ReaderFn &&fn)
    : os_(os), fn_(fn) {

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
}

void MulticastReceiver::start_reading() {
  char in_buffer[1024];
  unsigned int addrlen = sizeof(addr_);
  while (!stop_reading_) {
    int cnt = recvfrom(fd_, in_buffer, sizeof(in_buffer), MSG_DONTWAIT,
                       (struct sockaddr *)&addr_, &addrlen);

    if (cnt == 0) {
      break;
    }
    if (cnt > 0) {
      fn_({in_buffer, std::size_t(cnt)});
    }
  }
}
void MulticastReceiver::stop_reading() { stop_reading_ = true; }
} // namespace io