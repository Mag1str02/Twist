#include <wheels/core/source_location.hpp>

namespace wheels {

// Format: {file}:{function}[Line {line_number}]

std::ostream& operator<<(std::ostream& out, const SourceLocation& where) {
  out << where.File() << ":" << where.Function() << "[Line " << where.Line()
      << "]";
  return out;
}

}  // namespace wheels
