#ifndef BRCTL_NETLINK_H
#define BRCTL_NETLINK_H

#include "brctl.h"

#include <cstring>
#include <filesystem>
#include <iostream>

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

  static inline __attribute__((always_inline)) std::string to_str_(int err) {
    return std::move(std::string(strerror(err)));
  }
};

#endif // BRCTL_NETLINK_H
