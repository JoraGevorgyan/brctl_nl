#ifndef BRCTL_H
#define BRCTL_H

#include <iostream>

class Brctl {
private:
  int socket_fd_;

public:
  explicit Brctl() = default;
  int init();
  int show() const;
  int add(const std::string& br_name) const;
  int addif(const std::string& br_name, const std::string& iface_name) const;
  int del(const std::string& br_name) const;
  int delif(const std::string& br_name, const std::string& iface_name) const;
  ~Brctl() noexcept;
};

#endif // BRCTL_H
