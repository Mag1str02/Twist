#pragma once

#include <chrono>

#include <string>

namespace wheels::test {

// TODO: better
std::string FormatStderrForErrorMessage(const std::string& err);

double ToSeconds(std::chrono::milliseconds d);

std::string FormatSeconds(std::chrono::milliseconds d, size_t digits);

}  // namespace wheels::test
