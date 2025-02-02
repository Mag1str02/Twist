#if defined(__TWIST_SIM_ISOLATION__)

#include <twist/rt/sim/system/simulator.hpp>

#include <wheels/core/compiler.hpp>
#include <wheels/core/panic.hpp>

#include <new>

using twist::rt::sim::system::Simulator;

static void* New(std::size_t size) {
  Simulator* simulator = Simulator::TryCurrent();

  if ((simulator != nullptr) && (simulator->UserMemoryAllocator())) {
    // User memory
    return simulator->UserMalloc(size);
  } else {
    return std::malloc(size);
  }
}

static void Delete(void* ptr) {
  Simulator* simulator = Simulator::TryCurrent();

  if ((simulator != nullptr) && (simulator->UserMemoryAllocator())) {
    // User memory
    return simulator->UserFree(ptr);
  } else {
    std::free(ptr);
  }
}

//////////////////////////////////////////////////////////////////////

// Overload global new / delete

void* operator new(std::size_t s) {
  return New(s);
}

void* operator new(std::size_t /*s*/, std::align_val_t /*al*/) {
  WHEELS_PANIC("Aligned new not supported");
}

void operator delete(void* p) throw() {
  Delete(p);
}

void operator delete(void* p, std::size_t) throw() {
  Delete(p);
}

void operator delete(void* /*p*/, std::align_val_t /*al*/) throw() {
  WHEELS_PANIC("Aligned new not supported");
}

// Arrays

void* operator new[](std::size_t s) {
  return New(s);
}

void* operator new[](std::size_t /*s*/, std::align_val_t /*al*/) {
  WHEELS_PANIC("Aligned new[] not supported");
}

void operator delete[](void* p) throw() {
  Delete(p);
}

void operator delete[](void* p, std::size_t) throw() {
  Delete(p);
}

void operator delete[](void* /*p*/, std::align_val_t /*al*/) throw() {
  WHEELS_PANIC("Aligned new[] not supported");
}

#endif
