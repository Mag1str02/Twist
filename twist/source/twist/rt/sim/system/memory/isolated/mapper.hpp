#pragma once

#include <wheels/system/mmap.hpp>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

class MemoryMapper {
 public:
  MemoryMapper();

  void Assign(void* addr, size_t size);

  wheels::MutableMemView MutView() {
    return map_.MutView();
  }

  bool FixedAddress() const {
    return fixed_address_;
  }

 private:
  void AutoMmap();
  void Mmap(void* addr, size_t size);

 private:
  wheels::MmapAllocation map_;
  bool fixed_address_;
};

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
