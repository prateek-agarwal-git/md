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
  void exchange_test();
  void assert_true(const std::string &test_name, bool expression);

  void market_data_parser_test(const std::string &test_name,
                               const std::string &input,
                               const common::BinaryMarketData &);

  void response_reader_test();
  void tcp_client_server_test();

  void address_splitter_test();

  void run_side_book_tests();

  void bid_side_book_tests();
  void ask_side_book_tests();
  void quoter_test();
  std::ostream &os_;
};
} // namespace tests