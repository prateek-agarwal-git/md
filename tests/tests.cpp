#include "tests/tests.h"
#include "common_defs.h"
#include "double_comparators.h"
#include "exchange/exchange.h"
#include "io/multicast_receiver.h"
#include "io/multicast_sender.h"
#include "io/tcp_client.h"
#include "io/tcp_server.h"
#include "market_data_injector/market_data_parser.h"
#include "strategy/order_book.h"
#include "strategy/side_book.h"
#include "tests/test_server.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>
#include <thread>
#include <unistd.h>

int main() {
  std::string test_log = "test_" + std::to_string(getpid()) + ".txt";
  std::ofstream ofs(test_log, std::ios::out);
  tests::test_fixture F(ofs.is_open() ? ofs : std::cout);
  F.run_tests();
  return 0;
}

namespace tests {

void test_fixture::run_tests() {
  //  market_data_parser_test_1();
  //  market_data_parser_test_2();
  //  market_data_parser_test_3();
  //  multicast_sender_receiver_test();
  //  tcp_client_server_test();
  //  address_splitter_test();
  //  exchange_test();
  run_side_book_tests();
}
void test_fixture::run_side_book_tests() {
  bid_side_book_tests();
  ask_side_book_tests();
}

void test_fixture::ask_side_book_tests() {
  // side book is same. We just have to check whether the comparator is working
  // correctly. Still duplicating the bid side  tests for better protection
  // against regression is better.
  strategy::SideBook<common::LesserDouble> book;
  using mbp_entry_t = decltype(book)::mbp_entry;

  // basic tests for the top level.
  assert_true("ask_side_1", book.get_top_level() == mbp_entry_t{});
  book.update_book(1, 100.1, 10, 'N');
  assert_true("ask_side_2", book.get_top_level() == mbp_entry_t{100.1, 10, 1});
  book.update_book(1, 100.1, 10, 'C');
  assert_true("ask_side_3", book.get_top_level() == mbp_entry_t{});
  book.update_book(2, 100.1, 10, 'N');
  book.update_book(2, 100.1, 4, 'T');
  assert_true("ask_side_4", book.get_top_level() == mbp_entry_t{100.1, 6, 1});
  book.update_book(2, 100.1, 6, 'T');
  assert_true("ask_side_5", book.get_top_level() == mbp_entry_t{0, 0, 0});

  //  // quantity aggregation at top level
  book.update_book(3, 100.1, 10, 'N');
  book.update_book(4, 100.1, 5, 'N');
  book.update_book(5, 100.1, 3, 'N');
  assert_true("ask_side_6", book.get_top_level() == mbp_entry_t{100.1, 18, 3});

  // fill top 2 levels. Check the top level to ensure correct ordering.
  book.update_book(6, 100.1, 10, 'N');
  book.update_book(7, 100.2, 5, 'N');
  assert_true("ask_side_7", book.get_top_level() == mbp_entry_t{100.1, 28, 4});
  book.update_book(6, 100.1, 10, 'T');
  book.update_book(3, 100.1, 10, 'T');
  book.update_book(4, 100.1, 5, 'T');
  book.update_book(5, 100.1, 3, 'T');
  assert_true("ask_side_8", book.get_top_level() == mbp_entry_t{100.2, 5, 1});
  book.reset();

  // testing of clearing of intermediate levels
  book.update_book(8, 100.5, 10, 'N');
  book.update_book(9, 100.4, 10, 'N');
  book.update_book(10, 100.3, 10, 'N');
  book.update_book(11, 100.2, 10, 'N');
  book.update_book(12, 100.1, 10, 'N');
  book.update_book(9, 100.4, 10, 'C');
  assert_true("ask_side_9", book.get_top_level() == mbp_entry_t{100.1, 10, 1});
  book.update_book(11, 100.2, 10, 'C');
  assert_true("ask_side_10", book.get_top_level() == mbp_entry_t{100.1, 10, 1});
  // delete top level
  book.update_book(12, 100.1, 10, 'C');
  assert_true("ask_side_11", book.get_top_level() == mbp_entry_t{100.3, 10, 1});

  book.update_book(10, 100.3, 10, 'T');
  assert_true("ask_side_12", book.get_top_level() == mbp_entry_t{100.5, 10, 1});
  book.reset();

  // loss of lower levels in the book for current implementation

  book.update_book(13, 100.1, 10, 'N');
  book.update_book(14, 100.2, 10, 'N');
  book.update_book(15, 100.3, 10, 'N');
  book.update_book(16, 100.4, 10, 'N');
  book.update_book(17, 100.5, 10, 'N');
  // this order will be lost as we are maintaining top 5 levels
  book.update_book(18, 100.6, 10, 'N');

  book.update_book(13, 100.1, 10, 'C');
  book.update_book(14, 100.2, 10, 'C');
  book.update_book(15, 100.3, 10, 'C');
  book.update_book(16, 100.4, 10, 'C');

  assert_true("ask_side_13", book.get_top_level() == mbp_entry_t{100.5, 10, 1});
  book.update_book(17, 100.5, 10, 'C');
  assert_true("ask_side_14", book.get_top_level() == mbp_entry_t{});
}

void test_fixture::bid_side_book_tests() {
  strategy::SideBook<common::GreaterDouble> book;
  using mbp_entry_t = decltype(book)::mbp_entry;

  // basic tests for the top level.
  assert_true("bid_side_1", book.get_top_level() == mbp_entry_t{});
  book.update_book(1, 100.1, 10, 'N');
  assert_true("bid_side_2", book.get_top_level() == mbp_entry_t{100.1, 10, 1});
  book.update_book(1, 100.1, 10, 'C');
  assert_true("bid_side_3", book.get_top_level() == mbp_entry_t{});
  book.update_book(2, 100.1, 10, 'N');
  book.update_book(2, 100.1, 4, 'T');
  assert_true("bid_side_4", book.get_top_level() == mbp_entry_t{100.1, 6, 1});
  book.update_book(2, 100.1, 6, 'T');
  assert_true("bid_side_5", book.get_top_level() == mbp_entry_t{0, 0, 0});

  // quantity aggregation at top level
  book.update_book(3, 100.1, 10, 'N');
  book.update_book(4, 100.1, 5, 'N');
  book.update_book(5, 100.1, 3, 'N');
  assert_true("bid_side_6", book.get_top_level() == mbp_entry_t{100.1, 18, 3});

  // fill top 2 levels. Check the top level to ensure correct ordering.
  book.update_book(6, 100.1, 10, 'N');
  book.update_book(7, 100.2, 5, 'N');
  assert_true("bid_side_7", book.get_top_level() == mbp_entry_t{100.2, 5, 1});
  book.update_book(7, 100.2, 5, 'T');
  assert_true("bid_side_8", book.get_top_level() == mbp_entry_t{100.1, 28, 4});
  book.reset();

  // testing of clearing of intermediate levels
  book.update_book(8, 100.5, 10, 'N');
  book.update_book(9, 100.4, 10, 'N');
  book.update_book(10, 100.3, 10, 'N');
  book.update_book(11, 100.2, 10, 'N');
  book.update_book(12, 100.1, 10, 'N');
  book.update_book(9, 100.4, 10, 'C');
  assert_true("bid_side_9", book.get_top_level() == mbp_entry_t{100.5, 10, 1});
  book.update_book(11, 100.2, 10, 'C');
  assert_true("bid_side_10", book.get_top_level() == mbp_entry_t{100.5, 10, 1});
  book.update_book(8, 100.5, 10, 'C');
  assert_true("bid_side_11", book.get_top_level() == mbp_entry_t{100.3, 10, 1});
  book.update_book(10, 100.3, 10, 'T');
  assert_true("bid_side_12", book.get_top_level() == mbp_entry_t{100.1, 10, 1});
  book.reset();

  // loss of lower levels in the book for current implementation

  book.update_book(13, 100.5, 10, 'N');
  book.update_book(14, 100.4, 10, 'N');
  book.update_book(15, 100.3, 10, 'N');
  book.update_book(16, 100.2, 10, 'N');
  book.update_book(17, 100.1, 10, 'N');
  // this order will be lost as we are maintaining top 5 levels
  book.update_book(18, 100.0, 10, 'N');

  book.update_book(13, 100.5, 10, 'C');
  book.update_book(14, 100.4, 10, 'C');
  book.update_book(15, 100.3, 10, 'C');
  book.update_book(16, 100.2, 10, 'C');

  assert_true("bid_side_13", book.get_top_level() == mbp_entry_t{100.1, 10, 1});
  book.update_book(17, 100.1, 10, 'C');
  assert_true("bid_side_14", book.get_top_level() == mbp_entry_t{});
}

void test_fixture::exchange_test() {
  common::Request request{.order_id = 2,
                          .side = 'B',
                          .order_category = 'N',
                          .price = 100.00,
                          .quantity = 23};
  TestServer S(request);
  exchange::Exchange E(S, os_);
  E.set_read_cb();
  E.start_reading();
  auto response = S.response();

  assert_true("exchange_test",
              std::tolower(request.side) == response.side &&
                  std::tolower(request.order_category) ==
                      response.order_category &&
                  response.order_id == request.order_id &&
                  request.quantity == response.quantity &&
                  common::almost_equal(response.price, request.price));
}
void test_fixture::tcp_client_server_test() {
  std::string address_info = "127.0.0.1:8080";
  std::string received_response_at_client;
  auto client_read_cb = [&received_response_at_client](std::string_view data) {
    received_response_at_client = data;
  };
  std::string received_request_at_server;
  io::TCPServer server(address_info, os_);
  auto server_read_cb = [&received_request_at_server,
                         &server](std::string_view data) {
    received_request_at_server = data;
    std::string tmp = received_request_at_server;
    std::for_each(tmp.begin(), tmp.end(), [](auto &a) { a = toupper(a); });
    server(tmp);
  };
  server.set_read_cb(server_read_cb);
  std::thread read_thread{&io::TCPServer::start_reading, &server};
  sleep(2);
  io::TCPClient client(address_info, os_, client_read_cb);
  client("hello");
  assert_true("tcp_client_server_test",
              received_response_at_client == "HELLO" &&
                  received_request_at_server == "hello");

  std::thread stop_signal_thread{&io::TCPServer::stop_reading, &server};
  read_thread.join();
  stop_signal_thread.join();
}
std::string address = "127.0.0.1:8080";
void test_fixture::address_splitter_test() {
  auto [ip, port] = common::get_ip_port(address);
  assert_true("address_splitter_test", port == 8080 && ip == "127.0.0.1");
}

void test_fixture::multicast_sender_receiver_test() {
  const std::string multicast_address = "239.50.50.12:10812";
  std::string ans;
  auto cb = [&ans](std::string_view output) { ans += output; };
  io::MulticastSender ms(multicast_address, os_);
  io::MulticastReceiver mr(multicast_address, os_, cb);
  std::thread reader_thread{&io::MulticastReceiver::start_reading, &mr};
  ms("hello");
  ms(" ");
  ms("s");
  ms("a");
  ms("c");
  ms("c");
  ms("a");
  ms("d");
  ms("e");
  std::thread stop_reader{&io::MulticastReceiver::stop_reading, &mr};
  reader_thread.join();
  stop_reader.join();
  assert_true("multicast_sender_receiver_test", ans == "hello saccade");
}
void test_fixture::market_data_parser_test_2() {

  std::string input = "1609722840027700182      1374421476721608526    SCH    "
                      "   BUY CANCEL 100.0001       110";
  common::BinaryMarketData expected_bmd{.epoch = 1609722840027700182,
                                        .order_id = 1374421476721608526,
                                        .symbol = "SCH",
                                        .side = 'B',
                                        .order_category = 'C',
                                        .price = 100.0001,
                                        .quantity = 110};
  market_data_parser_test("market_data_parser_test_2", input, expected_bmd);
}

void test_fixture::market_data_parser_test_3() {

  std::string input = "1609722840027700182      1374421476721608526    SCH    "
                      "   BUY TRADE 100.01       110";
  common::BinaryMarketData expected_bmd{.epoch = 1609722840027700182,
                                        .order_id = 1374421476721608526,
                                        .symbol = "SCH",
                                        .side = 'B',
                                        .order_category = 'T',
                                        .price = 100.01,
                                        .quantity = 110};
  market_data_parser_test("market_data_parser_test_3", input, expected_bmd);
}

void test_fixture::market_data_parser_test_1() {

  std::string input = "1609722840027798182      7374421476721608526    SCS    "
                      "   SELL NEW                272.27       1";
  common::BinaryMarketData expected_bmd{.epoch = 1609722840027798182,
                                        .order_id = 7374421476721608526,
                                        .symbol = "SCS",
                                        .side = 'S',
                                        .order_category = 'N',
                                        .price = 272.27,
                                        .quantity = 1};
  market_data_parser_test("market_data_parser_test_1", input, expected_bmd);
}
void test_fixture::market_data_parser_test(
    const std::string &test_name, const std::string &input,
    const common::BinaryMarketData &expected_bmd) {
  char buffer[128]{};
  auto cb = [&buffer](std::string_view data) {
    std::memcpy(buffer, data.data(), data.size());
    return;
  };

  MDI::MarketDataParser mp(std::move(cb));
  std::stringstream ss(input);
  mp.read_data(ss);
  auto bmd = reinterpret_cast<common::BinaryMarketData *>(buffer);
  assert_true(test_name, *bmd == expected_bmd);
}

void test_fixture::assert_true(const std::string &test_name, bool expression) {
  std::cout << test_name << ": " << (expression ? "SUCCESS" : "FAILURE")
            << std::endl;
}

} // namespace tests
