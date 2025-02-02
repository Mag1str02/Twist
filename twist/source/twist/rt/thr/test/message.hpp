#pragma once

#include <memory>

namespace twist::rt::thr {

namespace test {

template <typename T>
class Message {
  struct Box {
    T content;

    Box(T&& c)
        : content(std::move(c)) {
    }
  };

  using BoxRef = std::unique_ptr<Box>;

 public:
  static Message New(T content) {
    return Message{std::make_unique<Box>(std::move(content))};
  }

  // Move-copy-constructible
  Message(Message&&) = default;

  // Non-copyable
  Message(const Message&) = delete;

  T Read() const {
    return box_->content;
  }

 private:
  explicit Message(BoxRef&& box)
      : box_(std::move(box)) {
  }

 private:
  BoxRef box_;
};

}  // namespace test

}  // namespace twist::rt::thr
