#include <wheels/core/exception.hpp>

#include <wheels/core/assert.hpp>

#include <stdexcept>

namespace wheels {

std::string What(std::exception_ptr e) {
  try {
    std::rethrow_exception(e);
  } catch (std::exception& e) {
    return {e.what()};
  } catch (...) {
    return "Wild exception";
  }
}

std::string CurrentExceptionMessage() {
  auto current = std::current_exception();

  WHEELS_VERIFY(current, "Not in exception context");

  return What(std::move(current));
}

}  // namespace wheels
