#include <wheels/test/fork.hpp>

#include <wheels/core/compiler.hpp>

#include <wheels/test/support/fork.hpp>

#include <wheels/test/helpers.hpp>
#include <wheels/test/here.hpp>
#include <wheels/test/framework.hpp>
#include <wheels/test/runtime.hpp>

#include <wheels/logging/logging.hpp>

#include <sys/types.h>
#include <unistd.h>

namespace wheels::test {

//////////////////////////////////////////////////////////////////////

class ForkedTestFailHandler : public ITestFailHandler {
 public:
  void Fail(const ITest&, const std::string& error) override {
    FlushPendingLogMessages();
    std::cerr << error << std::endl << std::flush;
    std::abort();
  }
};

ITestFailHandler& ForkedFailHandler() {
  static ForkedTestFailHandler instance;
  return instance;
}

//////////////////////////////////////////////////////////////////////

static void InstallForkedTestFailHandler() {
  Runtime::Access().InstallFailHandler(ForkedFailHandler());
}

static void RunTestInChildProcess(ITestPtr test, const GlobalOptions& options) {
  std::cout << "Executed in subprocess with pid = " << getpid() << std::endl;

  InstallForkedTestFailHandler();

  RunTestHere(test, options);

  FlushPendingLogMessages();
}

class LiveStdoutPrinter : public IByteStreamConsumer {
 public:
  void Consume(std::string_view chunk) override {
    std::cout.write(chunk.data(), chunk.length());
    total_bytes_consumed_ += chunk.length();
  }

  void HandleEof() override {
    if (total_bytes_consumed_ > 0) {
      std::cout << std::endl;
    }
  }

 private:
  size_t total_bytes_consumed_ = 0;
};

void RunTestWithFork(ITestPtr test, const GlobalOptions& options) {
  auto execute_test = [test, options]() {
    RunTestInChildProcess(test, options);
  };

  auto result = ExecuteWithFork(execute_test,
                                std::make_unique<LiveStdoutPrinter>(), nullptr);

  // Process result

  const auto& stderr = result.GetStderr();

  int exit_code;
  if (result.Exited(exit_code)) {
    if (exit_code != 0) {
      FAIL_TEST("Test subprocess terminated with non-zero exit code: "
                << exit_code
                << ", stderr: " << FormatStderrForErrorMessage(stderr));
    }
  }

  int signal;
  if (result.KilledBySignal(signal)) {
    if (stderr.empty()) {
      FAIL_TEST("Test subprocess terminated by signal: " << signal);
    } else {
      FAIL_TEST("Test subprocess terminated by signal "
                << signal
                << ", stderr: " << FormatStderrForErrorMessage(stderr));
    }
  }

  if (!stderr.empty()) {
    FAIL_TEST("Test produced stderr: " << FormatStderrForErrorMessage(stderr));
  }

  // Test completed!
}

}  // namespace wheels::test
