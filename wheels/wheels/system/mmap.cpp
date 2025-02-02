#include <wheels/system/mmap.hpp>

#include <wheels/system/page_size.hpp>

#include <wheels/core/assert.hpp>

#include <cerrno>
#include <cstring>

#include <unistd.h>

#include <sys/mman.h>

namespace wheels {

#define CHECK_RESULT(ret, error)                                  \
  WHEELS_VERIFY(ret != -1, error << " (errno = " << errno << ", " \
                                 << strerror(errno) << ")")

//////////////////////////////////////////////////////////////////////

static size_t PagesToBytes(size_t count) {
  return count * PageSize();
}

size_t MmapAllocation::PageSize() {
  return wheels::PageSize();
}

MmapAllocation MmapAllocation::AllocatePages(size_t count, void* hint) {
  size_t size = PagesToBytes(count);

  void* start = mmap(/*addr=*/hint, /*length=*/size,
                     /*prot=*/PROT_READ | PROT_WRITE,
                     /*flags=*/MAP_PRIVATE | MAP_ANONYMOUS,
                     /*fd=*/-1, /*offset=*/0);

  WHEELS_VERIFY(start != MAP_FAILED, "Cannot allocate " << count << " pages");

  return MmapAllocation{(char*)start, size};
}

MmapAllocation MmapAllocation::Acquire(MutableMemView view) {
  // TODO: check size and alignment
  return MmapAllocation{view.Data(), view.Size()};
}

void MmapAllocation::ProtectPages(size_t start_index, size_t count) {
  int ret = mprotect(/*addr=*/(void*)(start_ + PagesToBytes(start_index)),
                     /*len=*/PagesToBytes(count),
                     /*prot=*/PROT_NONE);
  CHECK_RESULT(
      ret, "Cannot protect pages [" << start_index << ", " << start_index + count << ")");
}

MmapAllocation::MmapAllocation(MmapAllocation&& that) {
  start_ = that.start_;
  size_ = that.size_;
  that.Reset();
}

MmapAllocation& MmapAllocation::operator=(MmapAllocation&& that) {
  Deallocate();
  start_ = that.start_;
  size_ = that.size_;
  that.Reset();
  return *this;
}

void MmapAllocation::Deallocate() {
  if (start_ == nullptr) {
    return;
  }

  int ret = munmap((void*)start_, size_);
  CHECK_RESULT(ret, "Cannot unmap allocated pages");
}

MutableMemView MmapAllocation::Release() {
  auto view = MutView();
  Reset();
  return view;
}

void MmapAllocation::Reset() {
  start_ = nullptr;
  size_ = 0;
}

}  // namespace wheels
