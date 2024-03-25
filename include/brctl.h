#ifndef BRCTL_H
#define BRCTL_H

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

class Brctl {
private:
  int socket_fd_;

public:
  explicit Brctl() = default;
  void init();
  static void show();
  void add(const std::string& br_name) const;
  void addif(const std::string& br_name, const std::string& iface_name) const;
  void del(const std::string& br_name) const;
  void delif(const std::string& br_name, const std::string& iface_name) const;
  ~Brctl() noexcept;

private:
  static bool is_bridge(const fs::directory_entry& dir_entry);
  static void show_bridge(const fs::directory_entry& dir_entry);
};

#endif // BRCTL_H
