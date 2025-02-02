#pragma once

#include <sstream>
#include <string>

namespace wheels {

class StringBuilder {
 public:
  template <typename T>
  StringBuilder& operator<<(const T& next) {
    out_ << next;
    return *this;
  }

  operator std::string() const {
    return out_.str();
  }

  std::string String() const {
    return out_.str();
  }

 private:
  std::ostringstream out_;
};

}  // namespace wheels
