#include <wheels/system/env.hpp>

#include <wheels/test/framework.hpp>

#include <iostream>

TEST_SUITE(Env) {
  SIMPLE_TEST(GetEnvVarOr) {
    auto value = wheels::GetEnvVarOr("19sf991dusdhfkjsdhfkjh1", "42");
    ASSERT_EQ(value, "42");
  }

  SIMPLE_TEST(GetUser) {
    auto user = wheels::GetUser();
    if (user.has_value()) {
      std::cout << "User: " << *user << std::endl;
    }
  }

  SIMPLE_TEST(GetTempPath) {
    auto temp_path = wheels::GetTempPath();
    if (temp_path.has_value()) {
      std::cout << "Temp dir path: " << *temp_path << std::endl;
    }
  }
}
