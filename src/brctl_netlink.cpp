#include "brctl_netlink.h"
#include "keywords.h"

#include <cerrno>
#include <linux/if.h>
#include <linux/if_bridge.h>
#include <sys/ioctl.h>
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

void BrctlNetlink::add_ioctl(const std::string &br_name) const {
  unsigned long arg[2] = {BRCTL_ADD_BRIDGE, (unsigned long)br_name.c_str()};
  call_ioctl_(arg, "adding " + br_name);
}

void BrctlNetlink::add(const std::string &br_name) const {
  auto *nlh = init_nlmsghdr_();
  nlh->nlmsg_type = RTM_NEWLINK;

  auto err = init_nlh_and_send_(nlh, br_name);
  free(nlh);
  const auto sub_msg = "trying to add " + br_name;
  check_response_(err, sub_msg);
}

void BrctlNetlink::del_ioctl(const std::string &br_name) const {
  unsigned long arg[2] = {BRCTL_DEL_BRIDGE, (unsigned long)br_name.c_str()};
  call_ioctl_(arg, "deleting " + br_name);
}

void BrctlNetlink::call_ioctl_(unsigned long *arg,
                               const std::string &msg_suf) const {
  auto ret = ioctl(socket_fd_, SIOCSIFBR, arg);
  if (ret < 0) {
    const auto msg = "Got an error(" + to_str_(errno) + ") when " + msg_suf;
    throw std::runtime_error(msg);
  }
}

void BrctlNetlink::del(const std::string &br_name) const {
  auto *nlh = init_nlmsghdr_();
  nlh->nlmsg_type = RTM_DELLINK;

  auto err = init_nlh_and_send_(nlh, br_name);
  free(nlh);
  const auto sub_msg = "trying to delete " + br_name;
  check_response_(err, sub_msg);
}

void BrctlNetlink::addif(const std::string &br_name,
                         const std::string &iface_name) const {
  // TODO complete this
  std::cout << "'Adding' " << iface_name << " to " << br_name << std::endl;
}

void BrctlNetlink::delif(const std::string &br_name,
                         const std::string &iface_name) const {
  // TODO complete this
  std::cout << "'Deleting' " << iface_name << " from " << br_name << std::endl;
}

int BrctlNetlink::init_nlh_and_send_(struct nlmsghdr *nlh,
                                     const std::string &br_name) const {
  assign_ifinfo_(nlh);
  assign_rtattr_(nlh, br_name.c_str(), br_name.size() + 1);

  return send_msg_(nlh);
}

struct nlmsghdr *BrctlNetlink::init_nlmsghdr_() {
  const auto mem_size = NLMSG_SPACE(sizeof(struct ifinfomsg));
  struct nlmsghdr *nlh = (struct nlmsghdr *)malloc(mem_size);
  memset(nlh, 0, mem_size);

  nlh->nlmsg_len = mem_size;
  nlh->nlmsg_pid = getpid();
  nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;

  return nlh;
}

void BrctlNetlink::assign_ifinfo_(struct nlmsghdr *nlh) {
  struct ifinfomsg *ifinfo = (struct ifinfomsg *)NLMSG_DATA(nlh);
  ifinfo->ifi_family = AF_BRIDGE;
  ifinfo->ifi_index = 0;
  ifinfo->ifi_change = 0;
}

void BrctlNetlink::assign_rtattr_(struct nlmsghdr *nlh, const char *br_name,
                                  std::size_t size) {
  struct rtattr *rta =
      (struct rtattr *)(((char *)nlh) + NLMSG_ALIGN(sizeof(struct ifinfomsg)));
  rta->rta_type = IFLA_IFNAME;
  rta->rta_len = RTA_LENGTH(size);
  memcpy(RTA_DATA(rta), br_name, size);
  nlh->nlmsg_len += RTA_ALIGN(rta->rta_len);
}

void BrctlNetlink::check_response_(ssize_t err,
                                   const std::string &sub_msg) const {
  const auto msg_suffix = ") when " + sub_msg;
  if (err != 0) {
    const auto msg =
        "After sending response got an error(" + to_str_(err) + msg_suffix;
    throw std::runtime_error(msg);
  }

  char buffer[kw::BUF_SIZE];
  memset(buffer, '\0', kw::BUF_SIZE);

  err = receive_msg_(buffer);
  if (err != 0) {
    const auto msg = "When trying to receive response got an error(" +
                     to_str_(err) + msg_suffix;
    throw std::runtime_error(msg);
  }
  // TODO parse the response(buffer)
}

ssize_t BrctlNetlink::send_msg_(const struct nlmsghdr *nlh) const {
  struct iovec iov;
  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;

  auto msg = get_msg_to_send_(&iov);
  auto len = sendmsg(socket_fd_, &msg, 0);
  if (len == -1 || len != nlh->nlmsg_len) {
    return errno;
  }
  return 0;
}

ssize_t BrctlNetlink::receive_msg_(char *buffer) const {
  struct iovec iov;
  iov.iov_base = buffer;
  iov.iov_len = kw::BUF_SIZE;

  auto msg = get_msg_to_receive_(&iov);

  auto len = recvmsg(socket_fd_, &msg, 0);
  if (len == -1) {
    return errno;
  }
  return 0;
}

struct msghdr BrctlNetlink::get_msg_to_send_(struct iovec *iov) {
  auto dest_addr = get_dest_for_msg_();

  struct msghdr msg;
  memset(&msg, 0, sizeof(msg));
  msg.msg_name = (void *)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;

  return msg;
}

struct sockaddr_nl BrctlNetlink::get_dest_for_msg_() {
  struct sockaddr_nl dest_addr;
  memset(&dest_addr, 0, sizeof(dest_addr));

  dest_addr.nl_family = AF_NETLINK;
  dest_addr.nl_pid = 0;
  dest_addr.nl_groups = 0;
  return dest_addr;
}

struct msghdr BrctlNetlink::get_msg_to_receive_(struct iovec *iov) {
  struct sockaddr_nl src_addr;

  struct msghdr msg;
  memset(&msg, 0, sizeof(msg));
  msg.msg_name = &src_addr;
  msg.msg_namelen = sizeof(src_addr);
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;

  return msg;
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

BrctlNetlink::~BrctlNetlink() noexcept {
  if (socket_fd_ >= 0) {
    close(socket_fd_);
  }
}
