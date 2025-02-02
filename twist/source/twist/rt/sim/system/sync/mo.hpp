#pragma once

#include <atomic>

namespace twist::rt::sim {

namespace system::sync {

inline bool IsLoadOrder(std::memory_order mo) {
  switch (mo) {
    case std::memory_order::relaxed:
    case std::memory_order::acquire:
    case std::memory_order::seq_cst:
      return true;
    default:
      return false;
  }
}

inline bool IsAcquireOrder(std::memory_order mo) {
  switch (mo) {
    case std::memory_order::acquire:
    case std::memory_order::acq_rel:
    case std::memory_order::seq_cst:
      return true;
    default:
      return false;
  }
}

inline bool IsStoreOrder(std::memory_order mo) {
  switch (mo) {
    case std::memory_order::relaxed:
    case std::memory_order::acq_rel:
    case std::memory_order::release:
    case std::memory_order::seq_cst:
      return true;
    default:
      return false;
  }
}

inline bool IsReleaseOrder(std::memory_order mo) {
  switch (mo) {
    case std::memory_order::release:
    case std::memory_order::acq_rel:
    case std::memory_order::seq_cst:
      return true;
    default:
      return false;
  }
}

inline const char* MemoryOrderName(std::memory_order mo) {
  switch (mo) {
    case std::memory_order::relaxed:
      return "relaxed";
    case std::memory_order::consume:
      return "consume";
    case std::memory_order::acquire:
      return "acquire";
    case std::memory_order::release:
      return "release";
    case std::memory_order::acq_rel:
      return "acq_rel";
    case std::memory_order::seq_cst:
      return "seq_cst";
    default:
      WHEELS_UNREACHABLE();
  }
}

}  // namespace system::sync

}  // namespace twist::rt::sim
