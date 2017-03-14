#ifndef CPPPC__TEST__TEST_BASE_H_
#define CPPPC__TEST__TEST_BASE_H_

#include <gtest/gtest.h>
#include <boost/geometry/algorithms/equals.hpp>
#include <boost/geometry/io/wkt/wkt.hpp>


namespace testing {
namespace internal {

enum GTestColor {
    COLOR_DEFAULT,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW
};

extern void ColoredPrintf(
  GTestColor color,
  const char* fmt,
  ...);

} // namespace internal
} // namespace testing

#define LOG_MESSAGE(...) do { \
  char buffer[300]; \
  const char * filepath = __FILE__; \
  const char * filebase = strrchr(filepath, '/'); \
  const char * filename = (filebase != 0) ? filebase + 1 : filepath; \
  sprintf(buffer, __VA_ARGS__); \
  testing::internal::ColoredPrintf( \
    testing::internal::COLOR_YELLOW, \
    "[=   LOG  =] %*s :%*d | %s \n", \
    24, filename, 4, __LINE__, buffer); \
} while(0)

// Fancy way to output a sequence of values to a string:
template <typename Iter>
std::string range_to_string(Iter first, Iter last) {
  std::ostringstream oss;
  std::copy(first, last,
            std::ostream_iterator<int>(oss, " "));
  return oss.str();
}

template<typename GeometryLeft, typename GeometryRight>
::testing::AssertionResult matchGeoEq(const GeometryLeft &left,
                                      const GeometryRight &right) {
  return boost::geometry::equals(left, right)
    ? ::testing::AssertionSuccess()
    : ::testing::AssertionFailure()
        << "\n    Geos not equal:\n      "
        << boost::geometry::wkt(left)
        << "\n        ==\n      "
        << boost::geometry::wkt(right)
        << "\n  ";
};

#endif // CPPPC__TEST__TEST_BASE_H_