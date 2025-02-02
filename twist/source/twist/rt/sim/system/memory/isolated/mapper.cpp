#include "mapper.hpp"

#include <wheels/core/assert.hpp>
#include <wheels/core/panic.hpp>

// std::getenv
#include <cstdlib>
#include <optional>
#include <ios>
#include <sstream>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

//////////////////////////////////////////////////////////////////////

#if defined(__TWIST_SIM_FIXED_USER_MEMORY_MAPPING__)

static void* StaticMmapAddress() {
  // https://github.com/llvm/llvm-project/blob/main/compiler-rt/lib/tsan/rtl/tsan_platform.h
  static void* const kAddress = (void*)0x0200000000ULL;

  return kAddress;
}

#endif

//////////////////////////////////////////////////////////////////////

static std::optional<void*> ParseHexAddr(char* addr_str) {
  try {
    std::stringstream ss;
    ss << addr_str;

    uintptr_t addr;
    ss >> std::hex >> addr;
    if (ss.eof()) {
      return (void*)addr;
    } else {
      return std::nullopt;
    }
  } catch (...) {
    return std::nullopt;
  }
}

static void* TryGetMmapAddressFromEnv() {
  static const char* kAddressVar = "TWIST_USER_MEMORY_ADDRESS";

  char* addr_str = std::getenv(kAddressVar);

  if (!addr_str) {
    return nullptr;  // Not presented
  }

  if (auto addr = ParseHexAddr(addr_str)) {
    return *addr;
  } else {
    WHEELS_PANIC("Cannot parse " << kAddressVar << ": " << addr_str);
  }
}

//////////////////////////////////////////////////////////////////////

static void* SelectMmapAddress() {
#if defined(__TWIST_SIM_FIXED_USER_MEMORY_MAPPING__)
  return StaticMmapAddress();
#endif

  if (void* env_addr = TryGetMmapAddressFromEnv()) {
    return env_addr;
  }

  return nullptr;  // Arbitrary
}

//////////////////////////////////////////////////////////////////////

static const size_t kMmapSize = 1024 * 1024 * 1024;

//////////////////////////////////////////////////////////////////////

static size_t ToPages(size_t size) {
  return 1 + size / wheels::MmapAllocation::PageSize();
}

MemoryMapper::MemoryMapper() {
  AutoMmap();
}

void MemoryMapper::Mmap(void* addr, size_t size) {
  size_t pages = ToPages(size);
  auto map = wheels::MmapAllocation::AllocatePages(pages, /*hint=*/addr);

  if (addr != nullptr) {
    WHEELS_VERIFY(map.Start() == addr,
                  "Cannot mmap requested memory address");
  }

  map_ = std::move(map);
  fixed_address_ = addr != nullptr;
}

void MemoryMapper::AutoMmap() {
  void* addr = SelectMmapAddress();
  Mmap(addr, kMmapSize);
}

void MemoryMapper::Assign(void* addr, size_t size) {
  Mmap(addr, size);
}

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
