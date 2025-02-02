#pragma once

#include <wheels/test/assert_failure.hpp>
#include <wheels/test/test.hpp>
#include <wheels/test/main.hpp>
#include <wheels/test/test_options.hpp>
#include <wheels/test/context.hpp>
#include <wheels/test/current.hpp>

#include <wheels/core/nullptr.hpp>
#include <wheels/core/preprocessor.hpp>
#include <wheels/core/string_builder.hpp>

#include <iostream>
#include <chrono>
#include <vector>

namespace wheels::test {

////////////////////////////////////////////////////////////////////////////////

void FailTest(const std::string& reason);
void FailTestByAssert(const AssertionFailure& failure);
void FailTestByException();

////////////////////////////////////////////////////////////////////////////////

#define ASSERT_TRUE(cond)                                      \
  if (!(cond)) {                                               \
    ::wheels::test::FailTestByAssert(ASSERTION_FAILURE(cond)); \
  }

#define ASSERT_TRUE_M(cond, message)                                      \
  if (!(cond)) {                                                          \
    ::wheels::test::FailTestByAssert(ASSERTION_FAILURE(cond) << message); \
  }

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_FALSE_M(cond, message) ASSERT_TRUE_M(!(cond), message)

#define ASSERT_EQ(x, y) ASSERT_TRUE((x) == (y))
#define ASSERT_NE(x, y) ASSERT_TRUE((x) != (y))

// Strict
#define ASSERT_GT(x, y) ASSERT_TRUE((x) > (y))
#define ASSERT_LT(x, y) ASSERT_TRUE((x) < (y))

// Non-strict
#define ASSERT_GE(x, y) ASSERT_TRUE((x) >= (y))
#define ASSERT_LE(x, y) ASSERT_TRUE((x) <= (y))

#define ASSERT_THROW(expr, exception)                                        \
  try {                                                                      \
    expr;                                                                    \
    ASSERT_TRUE_M(false, "Expected exception '" #exception "' not thrown")   \
  } catch (exception & expected) {                                           \
  } catch (...) {                                                            \
    ASSERT_TRUE_M(false,                                                     \
                  "Thrown unexpected exception, expected '" #exception "'"); \
  }

#define FAIL_TEST(error) \
  ::wheels::test::FailTest(::wheels::StringBuilder() << error)

////////////////////////////////////////////////////////////////////////////////

void RegisterTest(ITestPtr test);

////////////////////////////////////////////////////////////////////////////////

// Test suite provides separate namespace for bunch of simple test functions

#define TEST_SUITE(name)                    \
  namespace TestSuite##name {               \
    std::string GetCurrentTestSuiteName() { \
      return #name;                         \
    }                                       \
  }                                         \
  namespace TestSuite##name

#define TEST(name, options)                                         \
  void TestRoutine##name();                                         \
  struct Test##name : public ::wheels::test::ITest {                \
    std::string Name() const override {                             \
      return #name;                                                 \
    }                                                               \
    std::string Suite() const override {                            \
      return GetCurrentTestSuiteName();                             \
    }                                                               \
    ::wheels::test::TestOptions Options() const override {          \
      return options;                                               \
    }                                                               \
    void Run() override {                                           \
      TestRoutine##name();                                          \
    }                                                               \
  };                                                                \
  struct Test##name##Registrar {                                    \
    Test##name##Registrar() {                                       \
      ::wheels::test::RegisterTest(std::make_shared<Test##name>()); \
    }                                                               \
  };                                                                \
  static Test##name##Registrar test_##name##_registrar_;            \
  void TestRoutine##name()

#define SIMPLE_TEST(name) TEST(name, ::wheels::test::TestOptions{})

////////////////////////////////////////////////////////////////////////////////

}  // namespace wheels::test

#define RUN_ALL_TESTS()                                \
  int main(int argc, char** argv) {                    \
    wheels::test::RunTestsMain(argc, argv);            \
    return EXIT_SUCCESS;                               \
  }
