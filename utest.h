#ifndef CPP_UTILS_UTEST_H
#define CPP_UTILS_UTEST_H
#include <iostream>
#include <sstream>
#include <string>

#define UTest(name)                                                            \
  void test_function_##name(                                                   \
      [[maybe_unused]] utest::test_status_t &__test_status__)

#define run_test(name)                                                         \
  {                                                                            \
    utest::test_status_t status{false, 0};                                     \
    test_function_##name(status);                                              \
    utest::report(#name, status);                                              \
  }

#define uassert(expr)                                                          \
  if (utest::assert_("ASSERT", expr, #expr, __FILE__, __LINE__) != 0) {        \
    ++__test_status__.nb_assert_failed;                                        \
  }
#define uassert_equal(found, expect)                                           \
  if (utest::assert_equal_("ASSERT", found, expect, #found, #expect, __FILE__, \
                           __LINE__) != 0) {                                   \
    ++__test_status__.nb_assert_failed;                                        \
  }
#define urequire(expr)                                                         \
  if (utest::assert_("REQUIRE", expr, #expr, __FILE__, __LINE__) != 0) {       \
    __test_status__.require_failed = true;                                     \
    return;                                                                    \
  }

namespace utest {

struct test_status_t {
  bool require_failed = false;
  size_t nb_assert_failed = 0;
};

inline void report(std::string const &test_name, test_status_t const &status) {
  if (!status.require_failed && status.nb_assert_failed == 0) {
    std::cerr << "\033[0;32m[TEST PASSED] " << test_name << "\033[0m"
              << std::endl;
  } else {
    std::cerr << "\033[1;31m[TEST FAILED] " << test_name << ":\033[0m ";

    if (status.require_failed) {
      std::cerr << "failed on require." << std::endl;
    } else {
      std::cerr << status.nb_assert_failed << " assertion failed." << std::endl;
    }
  }
}

inline void error(std::string const &group, std::string const filename,
                  size_t line, std::string const &msg) {
  std::cerr << "\033[1;31m[" << group << " ERROR]:\033[0m " << filename << ":"
            << line << ": " << msg << std::endl;
}

inline int assert_(std::string const &group, bool expr,
                   std::string const &expr_str, std::string const &filename,
                   size_t line) {
  if (!expr) {
    error(group, filename, line, "`" + expr_str + "` evaluated to false.");
    return 1;
  }
  return 0;
}

inline int assert_equal_(std::string const &group, auto const &found,
                         auto const &expect, std::string const &lhs_str,
                         std::string const &rhs_str,
                         std::string const &filename, size_t line) {
  if (found != expect) {
    std::ostringstream oss;
    oss << "`" << lhs_str << "` != `" << rhs_str << "` -> found " << found
        << " expected " << expect << ".";
    error(group, filename, line, oss.str());
    return 1;
  }
  return 0;
}

} // namespace utest

#endif
