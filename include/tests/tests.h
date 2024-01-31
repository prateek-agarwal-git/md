#pragma once
#include <ostream>

namespace tests {

class test_fixture {
public:
  explicit test_fixture(std::ostream &os) : os_(os) {}
  void run_tests();

private:
  void run_market_data_parser_test();
  void assert_true(const std::string &test_name, bool expression);
  std::ostream &os_;
};
} // namespace tests