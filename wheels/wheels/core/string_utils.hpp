#pragma once

#include <wheels/core/noncopyable.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <ostream>

namespace wheels {

////////////////////////////////////////////////////////////

using Tokens = std::vector<std::string>;

Tokens Split(std::string str, char delimiter);

////////////////////////////////////////////////////////////

bool StartsWith(const std::string& str, std::string_view prefix);

////////////////////////////////////////////////////////////

namespace detail {

// TODO: reference?
template <typename T>
struct Quoted {
  T value;
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const Quoted<T> quoted) {
  out << "'" << quoted.value << "'";
  return out;
}

}  // namespace detail

template <typename T>
detail::Quoted<T> Quoted(const T& value) {
  return detail::Quoted<T>{value};
}

}  // namespace wheels
