#ifndef BRCTL_H
#define BRCTL_H

#include <string>

class Brctl {
protected:
  int socket_fd_;

public:
  explicit Brctl(): socket_fd_(-1) {}
  virtual void show() const = 0;
  virtual void add(const std::string &br_name) const = 0;
  virtual void addif(const std::string &br_name,
                     const std::string &iface_name) const = 0;
  virtual void del(const std::string &br_name) const = 0;
  virtual void delif(const std::string &br_name,
                     const std::string &iface_name) const = 0;
  virtual ~Brctl() = 0;
};

#endif // BRCTL_H
