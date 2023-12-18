#ifndef SK_TESTS_COMMON_TEST_SUITE_HPP
#define SK_TESTS_COMMON_TEST_SUITE_HPP

#include "decorators.hpp"

#include <iostream>

namespace sk::test {

class TestSuite {
  public:
    virtual ~TestSuite() = default;

    virtual void run_tests() = 0;

  protected:
    static void test_case(const std::string& title, const Delegate& delegate) {
        std::cout << std::format("========== Test {} ==========\n", title);
        delegate();
    };
};

}// namespace sk::test

#endif// SK_TESTS_COMMON_TEST_SUITE_HPP
