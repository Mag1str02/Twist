#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/static/thread_local/var.hpp>

#define TWISTED_STATIC_THREAD_LOCAL_MEMBER_DECLARE(M, name) \
  static twist::rt::sim::user::StaticThreadLocalVar<M> name

#define TWISTED_STATIC_THREAD_LOCAL_MEMBER_DEFINE(T, M, name) \
  twist::rt::sim::user::StaticThreadLocalVar<M> T::name{#name, /*member=*/true}

#else

#include <twist/rt/thr/static/thread_local/var.hpp>

#define TWISTED_STATIC_THREAD_LOCAL_MEMBER_DECLARE(M, name) \
  static thread_local twist::rt::thr::StaticThreadLocalVar<M> name

#define TWISTED_STATIC_THREAD_LOCAL_MEMBER_DEFINE(T, M, name) \
  thread_local twist::rt::thr::StaticThreadLocalVar<M> T::name{}

#endif
