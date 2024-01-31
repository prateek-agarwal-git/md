#include "tests/tests.h"
#include "io/multicast_receiver.h"
#include "io/multicast_sender.h"
#include "market_data_injector/market_data_parser.h"
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
  market_data_parser_test_1();
  market_data_parser_test_2();
  market_data_parser_test_3();
  multicast_sender_receiver_test();
  address_splitter_test();
}

void test_fixture::address_splitter_test() {
  std::string address = "127.0.0.1:8080";
  auto [ip,port] =common::get_ip_port(address);
  assert_true("address_splitter_test", port == 8080 && ip == "127.0.0.1");
}

void test_fixture::multicast_sender_receiver_test() {
  const std::string multicast_ip = "239.50.50.12";
  const uint16_t multicast_port = 10812;
  std::string ans;
  auto cb = [&ans](std::string_view output) { ans += output; };
  io::MulticastSender ms(multicast_ip, multicast_port, os_);
  io::MulticastReceiver mr(multicast_ip, multicast_port, os_, cb);
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
