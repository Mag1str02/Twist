#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/static/thread_local/var.hpp>
#include <twist/rt/sim/user/static/visit.hpp>

#define TWISTED_STATIC_THREAD_LOCAL_VAR(T, name) \
  static twist::rt::sim::user::StaticThreadLocalVar<T> name{#name}; \
  ___TWIST_VISIT_STATIC_VAR(name)

#else

#include <twist/rt/thr/static/thread_local/var.hpp>

#define TWISTED_STATIC_THREAD_LOCAL_VAR(T, name) static thread_local twist::rt::thr::StaticThreadLocalVar<T> name

#endif

#define TWISTED_STATIC_THREAD_LOCAL(T, name) TWISTED_STATIC_THREAD_LOCAL_VAR(T, name)
