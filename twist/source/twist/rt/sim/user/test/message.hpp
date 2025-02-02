#pragma once

#include <twist/rt/sim/user/assist/shared.hpp>

#include <memory>

namespace twist::rt::sim {

namespace test {

template <typename T>
class Message {
  struct Box {
    user::assist::Shared<T> content;

    Box(T c, wheels::SourceLocation loc)
        : content(std::move(c), loc) {
    }
  };

  using BoxRef = std::unique_ptr<Box>;

 public:
  static Message New(T content, wheels::SourceLocation loc = wheels::SourceLocation::Current()) {
    return Message{std::make_unique<Box>(std::move(content), loc)};
  }

  // Move-copy-constructible
  Message(Message&&) = default;

  // Non-copyable
  Message(const Message&) = delete;

  T Read(wheels::SourceLocation loc = wheels::SourceLocation::Current()) const {
    return box_->content.Read(loc);
  }

 private:
  explicit Message(BoxRef box)
      : box_(std::move(box)) {
  }

 private:
  BoxRef box_;
};

}  // namespace test

}  // namespace twist::rt::test
