#pragma once

#include <wheels/test/test.hpp>

#include <memory>

namespace wheels::test {

class ITestFilter {
 public:
  virtual ~ITestFilter() = default;

  virtual bool Accept(const ITestPtr& test) const = 0;
};

using ITestFilterPtr = std::shared_ptr<ITestFilter>;

ITestFilterPtr CreateTestFilter(std::string suite, std::string test);

}  // namespace wheels::test
