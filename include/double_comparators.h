#pragma once
#include <cmath>

namespace common {

// data in log files is upto 2 decimal places,this is kept
// arbitrarily accordingly as there could be precision error while
// reading log files (data coming from external source)
static constexpr double tolerance = 0.001;

inline bool almost_equal(double d1, double d2) {
  return std::abs(d1 - d2) < tolerance;
}

struct GreaterDouble {
  bool operator()(double d1, double d2) const {
    return !almost_equal(d1, d2) && d1 > d2;
  }
};

struct LesserDouble {
  bool operator()(double d1, double d2) const {
    return !almost_equal(d1, d2) && d1 < d2;
  }
};

} // namespace common