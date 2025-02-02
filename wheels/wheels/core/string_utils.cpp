#include <wheels/core/string_utils.hpp>

namespace wheels {

Tokens Split(std::string str, char delimiter) {
  Tokens tokens;
  while (true) {
    size_t pos = str.find(delimiter);
    if (pos == std::string::npos) {
      tokens.push_back(str);
      break;
    }
    tokens.push_back(str.substr(0, pos));
    str = str.substr(pos + 1);
  }
  return tokens;
}

bool StartsWith(const std::string& str, std::string_view prefix) {
  return prefix.compare(str.substr(0, prefix.length())) == 0;
}

}  // namespace wheels
