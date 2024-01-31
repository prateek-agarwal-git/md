#pragma once
#include "common_defs.h"
#include <ostream>

namespace tests {

class test_fixture {
public:
  explicit test_fixture(std::ostream &os) : os_(os) {}
  void run_tests();

private:
  void market_data_parser_test_1();
  void market_data_parser_test_2();
  void market_data_parser_test_3(); 
  void multicast_sender_receiver_test();
  void assert_true(const std::string &test_name, bool expression);

  void market_data_parser_test(const std::string &test_name,
                               const std::string &input,
                               const common::BinaryMarketData &);
  std::ostream &os_;
};
} // namespace tests