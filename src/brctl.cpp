#include "brctl.h"
#include "keywords.h"

#include <cerrno>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <unistd.h>

void Brctl::init() {
  socket_fd_ = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  if (socket_fd_ < 0) {
    const auto msg =
        "error(" + std::to_string(errno) + ") when creating the socket.";
    throw std::runtime_error(msg);
  }
}

void Brctl::show() {
  for (const auto &dir_entry : fs::directory_iterator(kw::BRIDGES_DIR)) {
    if (Brctl::is_bridge(dir_entry)) {
      Brctl::show_bridge(dir_entry);
    }
  }
}

bool Brctl::is_bridge(const fs::directory_entry &dir_entry) {
  if (!dir_entry.is_directory()) {
    return false;
  }
  return is_directory(dir_entry.path() / kw::BRIDGE);
}

void Brctl::show_bridge(const fs::directory_entry &dir_entry) {
  // TODO collect and show detailed information about bridge
  std::cout << dir_entry.path().filename() << std::endl;
}

void Brctl::add(const std::string &br_name) const {
  // TODO complete this
}

void Brctl::addif(const std::string &br_name,
                  const std::string &iface_name) const {
  // TODO complete this
  std::cout << "'Adding' " << iface_name << " to " << br_name << std::endl;
}

void Brctl::del(const std::string &br_name) const {
  // TODO complete this
}

void Brctl::delif(const std::string &br_name,
                  const std::string &iface_name) const {
  // TODO complete this
  std::cout << "'Deleting' " << iface_name << " from " << br_name << std::endl;
}

Brctl::~Brctl() noexcept {
  if (socket_fd_ >= 0) {
    close(socket_fd_);
  }
}
