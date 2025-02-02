#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/static/thread_local/ptr.hpp>
#include <twist/rt/sim/user/static/visit.hpp>

#define TWISTED_STATIC_THREAD_LOCAL_PTR(T, name) \
  static twist::rt::sim::user::StaticThreadLocalPtr name{ \
      twist::rt::sim::user::TypeDeductionHint<T>{},          \
      #name, [] { return nullptr; }}; \
  ___TWIST_VISIT_STATIC_VAR(name)

#define TWISTED_STATIC_THREAD_LOCAL_PTR_INIT(T, name, init) \
  static twist::rt::sim::user::StaticThreadLocalPtr name{ \
      twist::rt::sim::user::TypeDeductionHint<T>{},          \
      #name, [] { return init; }}; \
  ___TWIST_VISIT_STATIC_VAR(name)

#else

#define TWISTED_STATIC_THREAD_LOCAL_PTR(T, name) static thread_local T* name = nullptr

#define TWISTED_STATIC_THREAD_LOCAL_PTR_INIT(T, name, init) static thread_local T* name = init

#endif
