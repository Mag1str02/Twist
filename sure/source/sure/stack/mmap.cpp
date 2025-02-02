#include <sure/stack/mmap.hpp>

#include <utility>

using wheels::MmapAllocation;

namespace sure {

MmapStack::MmapStack(MmapAllocation allocation) : allocation_(std::move(allocation)) {
}

MmapStack MmapStack::AllocatePages(size_t count) {
  auto allocation = MmapAllocation::AllocatePages(count + 1);
  // Guard page
  allocation.ProtectPages(/*offset=*/0, /*count=*/1);
  return MmapStack{std::move(allocation)};
}

MmapStack MmapStack::AllocateBytes(size_t at_least) {
  const size_t page_size = MmapAllocation::PageSize();

  size_t pages = at_least / page_size;
  if (at_least % page_size != 0) {
    ++pages;
  }

  return MmapStack::AllocatePages(/*count=*/pages);
}

void* MmapStack::LowerBound() const {
  return allocation_.Start() + MmapAllocation::PageSize();
}

static StackView ToSpan(wheels::MutableMemView mem_view) {
  return {(std::byte*)mem_view.Data(), mem_view.Size()};
}

static wheels::MutableMemView ToMemView(StackView stack) {
  return {(char*)stack.data(), stack.size()};
}

StackView MmapStack::MutView() {
  return ToSpan(allocation_.MutView());
}

MmapStack MmapStack::Acquire(StackView view) {
  return MmapStack(MmapAllocation::Acquire(ToMemView(view)));
}

StackView MmapStack::Release() {
  return ToSpan(allocation_.Release());
}

}  // namespace sure
