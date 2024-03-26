#ifndef BRCTL_NETLINK_H
#define BRCTL_NETLINK_H

#include "brctl.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

namespace fs = std::filesystem;

class BrctlNetlink : public Brctl {
public:
  explicit BrctlNetlink() = default;
  void init();
  void show() const override;
  void add(const std::string &br_name) const override;
  void addif(const std::string &br_name,
             const std::string &iface_name) const override;
  void del(const std::string &br_name) const override;
  void delif(const std::string &br_name,
             const std::string &iface_name) const override;
  ~BrctlNetlink() noexcept;

private:
  static bool is_bridge_(const fs::directory_entry &dir_entry);
  static void show_bridge_(const fs::directory_entry &dir_entry);
  int add_bridge_(const std::string &br_name) const;

  int send_msg_(const struct nlmsghdr *nlh) const;
  ssize_t receive_msg_(char *buffer);
  static struct msghdr get_msg_to_send_(struct iovec *iov);
  static struct msghdr get_msg_to_receive_(struct iovec *iov);
  static struct sockaddr_nl get_dest_for_msg_();
  static struct nlmsghdr *init_nlmsghdr_();

  static inline __attribute__((always_inline)) std::string to_str_(int err) {
    return std::move(std::string(strerror(err)));
  }
};

#endif // BRCTL_NETLINK_H
