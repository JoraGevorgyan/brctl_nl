#include "brctl_netlink.h"
#include "keywords.h"

#include <cerrno>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <unistd.h>

void BrctlNetlink::init() {
  socket_fd_ = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (socket_fd_ < 0) {
    const auto msg = "error(" + to_str_(errno) + ") when creating the socket.";
    throw std::runtime_error(msg);
  }
}

void BrctlNetlink::show() const {
  for (const auto &dir_entry : fs::directory_iterator(kw::BRIDGES_DIR)) {
    if (is_bridge_(dir_entry)) {
      show_bridge_(dir_entry);
    }
  }
}

bool BrctlNetlink::is_bridge_(const fs::directory_entry &dir_entry) {
  if (!dir_entry.is_directory()) {
    return false;
  }
  return is_directory(dir_entry.path() / kw::BRIDGE);
}

void BrctlNetlink::show_bridge_(const fs::directory_entry &dir_entry) {
  // TODO collect and show detailed information about bridge
  std::cout << dir_entry.path().filename() << std::endl;
}

void BrctlNetlink::add(const std::string &br_name) const {
  auto err = add_bridge_(br_name);
  if (err != 0) {
    const auto msg =
        "error(" + to_str_(err) + ") when trying to add " + br_name + "!";
    throw std::runtime_error(msg);
  }
}

int BrctlNetlink::add_bridge_(const std::string &br_name) const {

}

void BrctlNetlink::addif(const std::string &br_name,
                  const std::string &iface_name) const {
  // TODO complete this
  std::cout << "'Adding' " << iface_name << " to " << br_name << std::endl;
}

void BrctlNetlink::del(const std::string &br_name) const {
  // TODO complete this
}

void BrctlNetlink::delif(const std::string &br_name,
                  const std::string &iface_name) const {
  // TODO complete this
  std::cout << "'Deleting' " << iface_name << " from " << br_name << std::endl;
}

BrctlNetlink::~BrctlNetlink() noexcept {
  if (socket_fd_ >= 0) {
    close(socket_fd_);
  }
}
