#include <wheels/test/support/fork.hpp>

#include <wheels/core/compiler.hpp>
#include <wheels/core/string_builder.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#if UNIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

namespace wheels {

////////////////////////////////////////////////////////////////////////////////

void Fail(const std::string& error_message) {
  static std::mutex mutex;
  std::lock_guard<std::mutex> locked(mutex);
  std::cerr << "'ExecuteWithFork' error: " << error_message << std::endl;
  std::abort();
}

void FailOnSystemError(const std::string& error_message) {
  Fail(StringBuilder() << error_message << " (system error code = " << errno
                       << +")");
}

////////////////////////////////////////////////////////////////////////////////

class GrowingBuffer {
 public:
  void Append(char* chunk, size_t length) {
    data_.insert(data_.end(), chunk, chunk + length);
  }

  size_t Size() const {
    return data_.size();
  }

  std::string ToString() const {
    return std::string(data_.begin(), data_.end());
  }

 private:
  std::vector<char> data_;
};

#if UNIX

////////////////////////////////////////////////////////////////////////////////

// Child process

class ParentProcessWatcher {
 public:
  ParentProcessWatcher(pid_t parent_pid)
      : poller_thread_(&ParentProcessWatcher::PollInBackground, parent_pid) {
    poller_thread_.detach();
  }

 private:
  static void PollInBackground(pid_t parent_pid) {
    while (true) {
      if (getppid() != parent_pid) {
        // Orphan child process automatically attached to init process with pid
        // == 1
        std::abort();
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

 private:
  std::thread poller_thread_;
};

void RedirectToPipe(int fd, int pipe_fd) {
  dup2(pipe_fd, fd);
}

void QuickExitWithSuccess() {
#if UNIX && !APPLE
  std::exit(EXIT_SUCCESS);
  // std::quick_exit(EXIT_SUCCESS); // why?
#else
  std::exit(EXIT_SUCCESS);
#endif
}

////////////////////////////////////////////////////////////////////////////////

// Parent process

class ForkAwarePipe {
 public:
  ForkAwarePipe() {
    if (pipe(fds_) != 0) {
      FailOnSystemError("Cannot create pipe");
    }
  }

  int AcquireWriterEnd() {
    close(fds_[0]);
    return fds_[1];
  }

  int AcquireReaderEnd() {
    close(fds_[1]);
    return fds_[0];
  }

 private:
  int fds_[2];
};

static size_t Read(int fd, char* buf, size_t count) {
  while (true) {
    ssize_t bytes_read = read(fd, buf, count);
    if (bytes_read >= 0) {
      return static_cast<size_t>(bytes_read);
    } else if (errno == EINTR) {
      continue;  // retry read
    } else {
      FailOnSystemError("Cannot read from file descriptor");
    }
  }
}

class PipeReader {
  static const size_t CHUNK_SIZE = 256;
  using ChunkBuffer = std::array<char, CHUNK_SIZE>;

 public:
  PipeReader(int pipe_fd, IByteStreamConsumerPtr consumer)
      : pipe_fd_(pipe_fd),
        live_consumer_(std::move(consumer)),
        reader_thread_(&PipeReader::ReadAll, this) {
  }

  explicit PipeReader(int pipe_fd) : PipeReader(pipe_fd, {}) {
  }

  std::string GetAll() {
    if (joined_) {
      return data_;
    }
    reader_thread_.join();
    data_ = all_data_buf_.ToString();
    joined_ = true;
    return data_;
  }

 private:
  char* GetChunkBuf() {
    return chunk_.begin();
  }

  void ReadAll() {
    while (true) {
      size_t bytes_read = Read(pipe_fd_, GetChunkBuf(), CHUNK_SIZE);
      if (bytes_read == 0) {
        break;  // Exhausted
      }
      all_data_buf_.Append(GetChunkBuf(), bytes_read);
      if (live_consumer_) {
        live_consumer_->Consume({GetChunkBuf(), bytes_read});
      }
    }
    if (live_consumer_) {
      live_consumer_->HandleEof();
    }
  }

 private:
  int pipe_fd_;
  IByteStreamConsumerPtr live_consumer_;

  ChunkBuffer chunk_;
  GrowingBuffer all_data_buf_;

  std::thread reader_thread_;

  bool joined_{false};
  std::string data_;
};

int AwaitTerminationAndGetStatus(pid_t child_pid) {
  int status;
  if (waitpid(child_pid, &status, 0) < 0) {
    FailOnSystemError("Failed to await child process");
  }
  return status;
}

////////////////////////////////////////////////////////////////////////////////

ForkResult ExecuteWithFork(std::function<void()> target,
                           IByteStreamConsumerPtr stdout_consumer,
                           IByteStreamConsumerPtr stderr_consumer) {
  ForkAwarePipe stdout_pipe;
  ForkAwarePipe stderr_pipe;

  pid_t parent_pid_before_fork = getpid();

  pid_t child_pid = fork();

  if (child_pid == 0) {
    // Child process

    ParentProcessWatcher parent_process_watcher(parent_pid_before_fork);

    RedirectToPipe(STDOUT_FILENO, stdout_pipe.AcquireWriterEnd());

    RedirectToPipe(STDERR_FILENO, stderr_pipe.AcquireWriterEnd());

    target();

    QuickExitWithSuccess();
  } else if (child_pid > 0) {
    // Parent process

    PipeReader stdout_reader(stdout_pipe.AcquireReaderEnd(),
                             std::move(stdout_consumer));

    PipeReader stderr_reader(stderr_pipe.AcquireReaderEnd(),
                             std::move(stderr_consumer));

    int status = AwaitTerminationAndGetStatus(child_pid);

    auto stdout = stdout_reader.GetAll();
    auto stderr = stderr_reader.GetAll();

    return ForkResult(status, std::move(stdout), std::move(stderr));
  } else {
    FailOnSystemError("Cannot fork process");
  }

  WHEELS_UNREACHABLE();
}

////////////////////////////////////////////////////////////////////////////////

bool ForkResult::Exited(int& exit_code) const {
  if (WIFEXITED(status_)) {
    exit_code = WEXITSTATUS(status_);
    return true;
  }
  return false;
}

bool ForkResult::KilledBySignal(int& signal) const {
  if (WIFSIGNALED(status_)) {
    signal = WTERMSIG(status_);
    return true;
  }
  return false;
}

#endif

}  // namespace wheels
