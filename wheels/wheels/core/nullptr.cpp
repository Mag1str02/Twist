#include <wheels/core/nullptr.hpp>

std::ostream& operator<<(std::ostream& out, std::nullptr_t) {
  return out << "nullptr";
}
