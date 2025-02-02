#include <wheels/test/emoji.hpp>

#include <vector>
#include <random>

namespace wheels::test {

namespace {

// Random utilities

size_t RandomIndex(size_t size) {
  static std::mt19937 twister{std::random_device{}()};
  return twister() % size;
}

template <typename T>
const T& PickRandom(const std::vector<T>& items) {
  return items[RandomIndex(items.size())];
}

}  // namespace

std::string_view GetSuccessEmoji() {
  static const std::vector<std::string_view> kCandidates = {
      "(=^ェ^=)",
      "ヽ(´ー｀)ノ",
      "(ﾉ^_^)ﾉ",
      "~(˘▾˘~)",
  };

  return PickRandom(kCandidates);
}

std::string_view GetFailEmoji() {
  static const std::vector<std::string_view> kCandidates = {
      "¯\\_(ツ)_/¯",
      "(ノಠ益ಠ)ノ",
      "[¬º-°]¬",
      "ヽ(｀⌒´)ﾉ",
  };

  return PickRandom(kCandidates);
}

}  // namespace wheels::test
