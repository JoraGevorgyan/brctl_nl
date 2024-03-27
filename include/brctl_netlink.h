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

  /// these two methods are just to get some result
  void add_ioctl(const std::string &br_name) const;
  void del_ioctl(const std::string &br_name) const;

  // TODO Unfortunately, I didn't managed to find out the reason why the needed
  // ones are not working, it runs without errors, but does nothing.
  void add(const std::string &br_name) const override;
  void del(const std::string &br_name) const override;
  void addif(const std::string &br_name,
             const std::string &iface_name) const override;
  void delif(const std::string &br_name,
             const std::string &iface_name) const override;
  ~BrctlNetlink() noexcept;

private:
  void call_ioctl_(unsigned long *arg, const std::string &msg_suf) const;
  int init_nlh_and_send_(struct nlmsghdr *nlh,
                         const std::string &br_name) const;
  static struct nlmsghdr *init_nlmsghdr_();
  static void assign_ifinfo_(struct nlmsghdr *nlh);
  static void assign_rtattr_(struct nlmsghdr *nlh, const char *br_name,
                             std::size_t size);

  void check_response_(ssize_t err, const std::string &sub_msg) const;
  ssize_t send_msg_(const struct nlmsghdr *nlh) const;
  ssize_t receive_msg_(char *buffer) const;

  static struct msghdr get_msg_to_send_(struct iovec *iov);
  static struct msghdr get_msg_to_receive_(struct iovec *iov);
  static struct sockaddr_nl get_dest_for_msg_();

  static bool is_bridge_(const fs::directory_entry &dir_entry);
  static void show_bridge_(const fs::directory_entry &dir_entry);

  static inline __attribute__((always_inline)) std::string
  to_str_(ssize_t err) {
    return std::move(std::string(strerror(err)));
  }
};

#endif // BRCTL_NETLINK_H
