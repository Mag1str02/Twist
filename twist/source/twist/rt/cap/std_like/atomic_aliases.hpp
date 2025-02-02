#pragma once

#include <twist/rt/cap/std_like/atomic.hpp>

#include <cstdint>
#include <cstddef>

namespace twist::rt::cap::std_like {

namespace atomic_aliases {

using atomic_bool = atomic<bool>;

using atomic_int = atomic<int>;
using atomic_uint = atomic<unsigned int>;

using atomic_long = atomic<long>;
using atomic_ulong = atomic<unsigned long>;

using atomic_llong = atomic<long long>;
using atomic_ullong = atomic<unsigned long long>;

using atomic_size_t = atomic<std::size_t>;

using atomic_intmax_t = atomic<std::intmax_t>;
using atomic_uintmax_t = atomic<std::uintmax_t>;

using atomic_ptrdiff_t = atomic<std::ptrdiff_t>;

using atomic_intptr_t = atomic<std::intptr_t>;
using atomic_uintptr_t = atomic<std::uintptr_t>;

// Fixed-width integer types

using atomic_int8_t = atomic<std::int8_t>;
using atomic_uint8_t = atomic<std::uint8_t>;

using atomic_int16_t = atomic<std::int16_t>;
using atomic_uint16_t = atomic<std::uint16_t>;

using atomic_int32_t = atomic<std::int32_t>;
using atomic_uint32_t = atomic<std::uint32_t>;

using atomic_int64_t = atomic<std::int64_t>;
using atomic_uint64_t = atomic<std::uint64_t>;

}  // namespace atomic_aliases

}  // namespace twist::rt::cap::std_like
