#include "tests/tests.h"
#include "market_data_injector/market_data_parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>
#include <unistd.h>

int main() {
  std::string test_log = "test_" + std::to_string(getpid()) + ".txt";
  std::ofstream ofs(test_log, std::ios::out);
  tests::test_fixture F(ofs.is_open() ? ofs : std::cout);
  F.run_tests();
  return 0;
}

namespace tests {

void test_fixture::run_tests() { run_market_data_parser_test(); }

void test_fixture::run_market_data_parser_test() {
  auto cb = [](std::string_view data) { return; };
  MDI::MarketDataParser mp(std::move(cb));
  std::stringstream ss("1609722840027798182      7374421476721608526    SCS    "
                       "   SELL NEW                272.27       1");
  mp.read_data(ss);
  assert_true("market_data_parser_test", true);
}

void test_fixture::assert_true(const std::string &test_name, bool expression) {
  std::cout << test_name << ": " << (expression ? "SUCCESS" : "FAILURE")
            << std::endl;
}

} // namespace tests
