#pragma once

#include <wheels/memory/view.hpp>

#include <cstdlib>

namespace wheels {

inline void HashCombine(size_t& digest, size_t hash_value) {
  // https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine
  digest ^= (hash_value + 0x9e3779b9 + (digest << 6) + (digest >> 2));
}

inline size_t HashRange(size_t seed, ConstMemView range) {
  size_t digest = seed;
  for (const char* ptr = range.Begin(); ptr != range.End(); ++ptr) {
    HashCombine(digest, (size_t)*ptr);
  }
  return digest;
}

}  // namespace wheels
