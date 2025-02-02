#pragma once

/*
 * Annotations for memory checks
 *
 * void twist::assist::MemoryAccess(void* addr, size_t size);
 *
 * void twist::assist::Access(T* ptr);
 *
 * twist::assist::Ptr<T>
 *
 * Usage:
 *
 * Node* top = top_.load(std::memory_order::acquire);
 * Node* top_next = twist::assist::Ptr(top)->next;
 *
 */

#include <twist/rt/cap/assist/memory.hpp>

namespace twist::assist {

using rt::cap::assist::New;
using rt::cap::assist::MemoryAccess;
using rt::cap::assist::Access;
using rt::cap::assist::Ptr;

}  // namespace twist::assist
