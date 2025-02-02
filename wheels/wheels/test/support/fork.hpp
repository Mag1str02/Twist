#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace wheels {

////////////////////////////////////////////////////////////////////////////////

struct IByteStreamConsumer {
  virtual ~IByteStreamConsumer() = default;
  virtual void Consume(std::string_view chunk) = 0;
  virtual void HandleEof() = 0;
};

using IByteStreamConsumerPtr = std::unique_ptr<IByteStreamConsumer>;

////////////////////////////////////////////////////////////////////////////////

class ForkResult {
 public:
  ForkResult(int status, std::string std_out, std::string std_err)
      : status_(status),
        stdout_(std::move(std_out)),
        stderr_(std::move(std_err)) {
  }

  bool Exited(int& exit_code) const;
  bool KilledBySignal(int& signal) const;

  const std::string& GetStdout() const {
    return stdout_;
  }

  const std::string& GetStderr() const {
    return stderr_;
  }

 private:
  int status_;
  std::string stdout_;
  std::string stderr_;
};

////////////////////////////////////////////////////////////////////////////////

ForkResult ExecuteWithFork(std::function<void()> target,
                           IByteStreamConsumerPtr stdout_consumer,
                           IByteStreamConsumerPtr stderr_consumer);

}  // namespace wheels
