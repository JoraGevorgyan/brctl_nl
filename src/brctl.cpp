#include "brctl.h"

#include <cerrno>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>

int Brctl::init() {
  socket_fd_ = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  if (socket_fd_ < 0) {
    return errno;
  }
  return 0;
}

int Brctl::show() const {
  // TODO complete this
  return 0;
}

int Brctl::add(const std::string &br_name) const {
  // TODO complete this
  return 0;
}

int Brctl::addif(const std::string &br_name,
                 const std::string &iface_name) const {
  // TODO complete this
  std::cout << "'Adding' " << iface_name << " to " << br_name << std::endl;
  return 0;
}

int Brctl::del(const std::string &br_name) const {
  // TODO complete this
  return 0;
}

int Brctl::delif(const std::string &br_name,
                 const std::string &iface_name) const {
  // TODO complete this
  std::cout << "'Deleting' " << iface_name << " from " << br_name << std::endl;
  return 0;
}

Brctl::~Brctl() noexcept {
  if (socket_fd_ >= 0) {
    close(socket_fd_);
  }
}
