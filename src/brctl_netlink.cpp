#include "brctl_netlink.h"
#include "keywords.h"

#include <cerrno>
#include <sys/socket.h>
#include <unistd.h>

void BrctlNetlink::init() {
  socket_fd_ = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (socket_fd_ < 0) {
    const auto msg = "error(" + to_str_(errno) + ") when creating the socket.";
    throw std::runtime_error(msg);
  }
}

int BrctlNetlink::send_msg_(const struct nlmsghdr *nlh) const {
  struct iovec iov;
  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;

  auto msg = get_msg_to_send_(&iov);
  return sendmsg(socket_fd_, &msg, 0);
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

int BrctlNetlink::receive_msg_(char *buffer) {
  struct iovec iov;
  iov.iov_base = buffer;
  iov.iov_len = kw::BUF_SIZE;

  auto msg = get_msg_to_receive_(&iov);

  return recvmsg(socket_fd_, &msg, 0);
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
  auto *nlh = init_nlmsghdr_();

  struct ifinfomsg *ifinfo = (struct ifinfomsg *)NLMSG_DATA(nlh);
  ifinfo->ifi_family = AF_BRIDGE;
  ifinfo->ifi_index = 0;
  ifinfo->ifi_change = 0;

  struct rtattr *rta =
      (struct rtattr *)(((char *)nlh) + NLMSG_ALIGN(sizeof(struct ifinfomsg)));
  rta->rta_type = IFLA_IFNAME;
  rta->rta_len = RTA_LENGTH(br_name.size() + 1);
  memcpy(RTA_DATA(rta), br_name.c_str(), br_name.size() + 1);
  nlh->nlmsg_len += RTA_ALIGN(rta->rta_len);

  auto err = send_msg_(nlh);
  free(nlh);
  return err;
}

struct nlmsghdr *BrctlNetlink::init_nlmsghdr_() {
  const auto mem_size = NLMSG_SPACE(sizeof(struct ifinfomsg));
  struct nlmsghdr *nlh = (struct nlmsghdr *)malloc(mem_size);
  memset(nlh, 0, mem_size);

  nlh->nlmsg_len = mem_size;
  nlh->nlmsg_pid = getpid();
  nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
  nlh->nlmsg_type = RTM_NEWLINK;

  return nlh;
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
