#include <wheels/test/test.hpp>

namespace wheels::test {

class Test : public ITest {
 public:
  Test(TestRoutine routine, const std::string& name, const std::string& suite,
       TestOptions options)
      : name_(name), suite_(suite), body_(routine), options_(options) {
  }

  std::string Name() const override {
    return name_;
  }

  std::string Suite() const override {
    return suite_;
  }

  TestOptions Options() const override {
    return options_;
  }

  void Run() override {
    body_();
  }

 private:
  std::string name_;
  std::string suite_;
  TestRoutine body_;
  TestOptions options_;
};

ITestPtr MakeTest(TestRoutine routine, const std::string& name,
                  const std::string& suite, TestOptions options) {
  return std::make_shared<Test>(routine, name, suite, options);
}

}  // namespace wheels::test