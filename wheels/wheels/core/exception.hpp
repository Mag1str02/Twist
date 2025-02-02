#pragma once

#include <exception>
#include <string>

namespace wheels {

std::string What(std::exception_ptr e);

std::string CurrentExceptionMessage();

}  // namespace wheels
