#include "argparse.h"
#include "brctl.h"
#include "keywords.h"

argparse::ArgumentParser init_parser() {
  argparse::ArgumentParser parser(kw::BIN, "ethernet bridge administration");

  const std::string dash = "--";
  parser.add_argument()
      .name(dash + kw::SHOW)
      .description("show a list of bridges");
  parser.add_argument().name(dash + kw::ADD).description("add bridge");
  parser.add_argument()
      .name(dash + kw::ADDIF)
      .description("add interface to bridge");
  parser.add_argument().name(dash + kw::DEL).description("delete bridge");
  parser.add_argument()
      .name(dash + kw::DELIF)
      .description("delete interface from bridge");

  parser.enable_help(); // TODO beautify the help page
  return parser;
}

std::vector<std::string> get_args(argparse::ArgumentParser &parser,
                                  const char *key, std::size_t exp_num) {
  auto args = parser.get<std::vector<std::string>>(key);
  if (args.size() != exp_num) {
    std::cerr << key << " requires " << exp_num << " argument(s)!" << std::endl;
    return {};
  }
  // TODO ensure that given names are valid
  return args;
}

void run_brctl(argparse::ArgumentParser &parser) {
  if (parser.exists(kw::SHOW)) {
    Brctl::show();
  }
  Brctl brctl{};
  brctl.init();

  do {
    if (parser.exists(kw::ADD)) {
      auto args = get_args(parser, kw::ADD, kw::ONE);
      if (!args.empty()) {
        brctl.add(args[0]);
      }
      break;
    }
    if (parser.exists(kw::ADDIF)) {
      auto args = get_args(parser, kw::ADDIF, kw::TWO);
      if (args.empty()) {
        brctl.addif(args[0], args[1]);
      }
      break;
    }
    if (parser.exists(kw::DEL)) {
      auto args = get_args(parser, kw::DEL, kw::ONE);
      if (args.empty()) {
        brctl.del(args[0]);
      }
      break;
    }
    if (parser.exists(kw::DELIF)) {
      auto args = get_args(parser, kw::DELIF, kw::TWO);
      if (args.empty()) {
        brctl.delif(args[0], args[1]);
      }
      break;
    }
  } while (false);

  parser.print_help();
}

int main(int argc, const char *argv[]) {
  auto parser = init_parser();

  auto err = parser.parse(argc, argv);
  if (err) {
    std::cout << err << std::endl;
    return 1;
  }
  try {
    run_brctl(parser);
  } catch (const std::filesystem::filesystem_error& err) {
    std::cerr << err.what() << std::endl;
    return 1;
  } catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Unknown error!" << std::endl;
    return 1;
  }
  return 0;
}
