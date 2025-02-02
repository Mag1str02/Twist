#pragma once

#include "size_classes.hpp"

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

//////////////////////////////////////////////////////////////////////

enum class BlockState : uint8_t {
  Allocated,
  Available,
};

//////////////////////////////////////////////////////////////////////

using AllocId = uint32_t;

//////////////////////////////////////////////////////////////////////

struct BlockHeader {
  BlockState state;
  SizeClass klass;
  bool service;
  uint32_t user_size;
  AllocId alloc_id;
  uint32_t canary;
};

//////////////////////////////////////////////////////////////////////

struct Block {
  BlockHeader header;
  Block* next;  // NB: only for freed blocks

  char* Data() const {
    return (char*)this + sizeof(BlockHeader);
  }

  SizeClass Class() const {
    return header.klass;
  }

  size_t Size() const {
    return SizeClassifier::BlockSize(Class());
  }

  size_t UserSize() const {
    return header.user_size;
  }

  char* DataEnd() const {
    return Data() + Size();
  }

  char* UserDataEnd() const {
    return Data() + UserSize();
  }

  uint32_t Canary() const {
    return header.canary;
  }
};

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
