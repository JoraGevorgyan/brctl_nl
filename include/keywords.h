#ifndef KEYWORDS_H
#define KEYWORDS_H

namespace kw {

/// command line arguments and binary name
constexpr auto BIN = "brctl_nl";
constexpr auto SHOW = "show";
constexpr auto ADD = "add";
constexpr auto ADDIF = "addif";
constexpr auto DEL = "del";
constexpr auto DELIF = "delif";

/// constant numbers
constexpr auto ONE = 1;
constexpr auto TWO = 2;
constexpr auto BUF_SIZE = 4096;

/// hardcoded paths
constexpr auto BRIDGES_DIR = "/sys/class/net/";
constexpr auto BRIDGE = "bridge";

} // end of namespace kw

#endif // KEYWORDS_H
