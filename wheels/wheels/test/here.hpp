#pragma once

#include <wheels/test/test.hpp>

namespace wheels::test {

// Here = in current process
void RunTestHere(const ITestPtr& test, const GlobalOptions& options);

}  // namespace wheels::test
