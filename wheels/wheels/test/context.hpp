#pragma once

#include <wheels/core/assert.hpp>

#include <wheels/core/string_utils.hpp>

#include <string>
#include <any>

namespace wheels::test {

std::any GetContextImpl(const std::string& key);

template <typename T>
T GetContext(const std::string& key) {
  auto value = GetContextImpl(key);
  WHEELS_VERIFY(value.has_value(),
                "Key " << Quoted(key) << " not found in test context");
  return std::any_cast<T>(value);
}

void SetContextImpl(const std::string& key, std::any value);

template <typename T>
void SetContext(const std::string& key, T value) {
  SetContextImpl(key, {value});
}

inline bool HasContext(const std::string& key) {
  return wheels::test::GetContextImpl(key).has_value();
}

}  // namespace wheels::test
