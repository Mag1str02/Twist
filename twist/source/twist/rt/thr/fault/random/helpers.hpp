#pragma once

#include <wheels/intrusive/list.hpp>

#include <twist/rt/thr/fault/random/range.hpp>

#include <vector>

namespace twist::rt::thr::fault {

template <typename T>
T* UnlinkRandomItem(wheels::IntrusiveList<T>& items) {
  auto iterator = items.begin();
  std::advance(iterator, RandomUInteger(items.Size() - 1));
  auto* item = iterator.Item();
  items.Unlink(item);
  return item;
}

// Fisher-Yates shuffle algorithm
template <typename T>
void RandomShuffleInplace(std::vector<T>& items) {
  if (items.empty()) {
    return;  // nothing to do
  }

  for (size_t i = items.size() - 1; i > 0; --i) {
    size_t j = RandomUInteger(i);
    if (i != j) {
      std::swap(items[i], items[j]);
    }
  }
}

template <typename T>
std::vector<T*> ShuffleToVector(wheels::IntrusiveList<T>& items) {
  std::vector<T*> shuffled;
  while (!items.IsEmpty()) {
    shuffled.push_back(items.PopFront());
  }
  RandomShuffleInplace(shuffled);
  return shuffled;
}

}  // namespace twist::rt::thr::fault
