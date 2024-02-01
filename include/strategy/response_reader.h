#pragma once
#include "common_defs.h"
#include <cstdint>
#include <ostream>
#include <string_view>

namespace strategy {
struct ResponseReader {
  ResponseReader(std::ostream &log) : log_(log) {}
  void operator()(std::string_view data) {
    auto response = *reinterpret_cast<const common::Response *>(data.data());
    ++response_counter_;
    log_ <<"response_counter="<< response_counter_ << " " <<response;
  }

private:
  std::ostream &log_;
  std::uint32_t response_counter_{};
};

} // namespace strategy