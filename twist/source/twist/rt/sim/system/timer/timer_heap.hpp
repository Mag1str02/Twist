#pragma once

#include "timer.hpp"

#include <wheels/core/assert.hpp>

#include <algorithm>
#include <array>
#include <tuple>

namespace twist::rt::sim {

namespace system {

class TimerHeap {
  static const constexpr size_t kTimerLimit = 256;

 public:
  TimerHeap() {
    tree_[0] = nullptr;
  }

  void Insert(Timer* timer) {
    WHEELS_VERIFY(size_ + 1 < kTimerLimit, "Timer limit reached: " << kTimerLimit);

    ++size_;
    tree_[size_] = timer;
    SiftUp(size_);
  }

  bool IsActive(Timer* timer) const {
    return timer->meta.loc > 0;
  }

  void Remove(Timer* timer) {
    size_t index = timer->meta.loc;
    WHEELS_VERIFY(tree_[index] == timer, "Invalid timer index");
    timer->meta.loc = 0;
    tree_[index] = tree_[size_];
    --size_;
    SiftDown(index);
  }

  bool IsEmpty() const {
    return size_ == 0;
  }

  bool NonEmpty() const {
    return !IsEmpty();
  }

  // Precondition: NonEmpty()
  Timer* ExtractMin() {
    WHEELS_ASSERT(!IsEmpty(), "ExtractMin on empty timer heap");

    Timer* min = tree_[1];

    tree_[1] = tree_[size_];
    --size_;
    if (size_ > 0) {
      SiftDown(1);
    }

    return min;
  }

  // Precondition: NonEmpty()
  Timer* Min() const {
    WHEELS_ASSERT(!IsEmpty(), "Min on empty timer heap");
    return tree_[1];
  }

 private:
  // NB: 1-based indices

  static size_t Parent(size_t i) {
    return i / 2;
  }

  static size_t LeftChild(size_t i) {
    return i * 2;
  }

  static size_t RightChild(size_t i) {
    return i * 2 + 1;
  }

  static bool IsRoot(size_t i) {
    return i == 1;
  }

  bool IsLeaf(size_t i) const {
    return i * 2 > size_;
  }

  static bool Less(Timer* lhs, Timer* rhs) {
    return std::tie(lhs->when, lhs->meta.id) < std::tie(rhs->when, rhs->meta.id);
  }

  void SiftUp(size_t index) {
    while (!IsRoot(index)) {
      size_t parent_index = Parent(index);

      if (Less(tree_[index], tree_[parent_index])) {
        std::swap(tree_[index], tree_[parent_index]);
        UpdateTimerMetadata(index);
        index = parent_index;
      } else {
        break;
      }
    }

    UpdateTimerMetadata(index);
  }

  void SiftDown(size_t index) {
    while (!IsLeaf(index)) {
      size_t min = index;

      size_t left = LeftChild(index);
      size_t right = RightChild(index);

      if (left <= size_) {
        if (Less(tree_[left], tree_[min])) {
          min = left;
        }
      }

      if (right <= size_) {
        if (Less(tree_[right], tree_[min])) {
          min = right;
        }
      }

      if (min != index) {
        std::swap(tree_[index], tree_[min]);
        UpdateTimerMetadata(index);
        index = min;
      } else {
        break;
      }
    }

    UpdateTimerMetadata(index);
  }

  void UpdateTimerMetadata(size_t index) {
    tree_[index]->meta.loc = index;
  }

 private:
  std::array<Timer*, kTimerLimit> tree_;
  size_t size_ = 0;
};

}  // namespace system

}  // namespace twist::rt::sim
