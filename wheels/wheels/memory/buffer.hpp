#pragma once

#include <wheels/memory/view.hpp>

#include <vector>

namespace wheels {

struct GrowingBuffer {
 public:
  GrowingBuffer() = default;

  void Reserve(size_t capacity) {
    buf_.reserve(capacity);
  }

  void Clear() {
    buf_.resize(0);
  }

  void Append(std::string_view data) {
    Append(ConstMemView{data.begin(), data.size()});
  }

  void Append(ConstMemView data) {
    ReserveForAppend(data.Size());
    buf_.insert(buf_.end(), data.Begin(), data.End());
  }

  ConstMemView View() const {
    return {buf_.data(), buf_.size()};
  }

  void ShrinkToFit() {
    buf_.shrink_to_fit();
  }

  size_t Size() const {
    return buf_.size();
  }

 private:
  void ReserveForAppend(size_t size) {
    buf_.reserve(buf_.size() + size);
  }

 private:
  std::vector<char> buf_;
};

}  // namespace wheels
