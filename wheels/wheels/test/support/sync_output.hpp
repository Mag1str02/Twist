#pragma once

#include <iostream>
#include <mutex>

namespace wheels {

class SyncOutput {
 private:
  using Lock = std::unique_lock<std::mutex>;

  class Proxy {
   public:
    Proxy(std::ostream& out, Lock lock) : out_(out), lock_(std::move(lock)) {
    }

    Proxy(Proxy&& that) = default;

    Proxy(const Proxy& that) = delete;
    Proxy& operator=(const Proxy& that) = delete;

    template <typename T>
    Proxy& operator<<(const T& v) {
      out_ << v;
      return *this;
    }

    // Support std::endl
    Proxy& operator<<(std::ostream& (*f)(std::ostream&)) {
      out_ << f;
      return *this;
    }

   private:
    std::ostream& out_;
    Lock lock_;
  };

 public:
  SyncOutput(std::ostream& out) : out_(out) {
  }

  template <typename T>
  Proxy operator<<(const T& v) {
    Lock lock(mutex_);
    out_ << v;
    return Proxy(out_, std::move(lock));
  }

 private:
  std::ostream& out_;
  std::mutex mutex_;
};

SyncOutput& SyncCout();
SyncOutput& SyncCerr();

}  // namespace wheels
