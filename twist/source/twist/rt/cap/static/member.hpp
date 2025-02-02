#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/static/var.hpp>

#define TWISTED_STATIC_MEMBER_DECLARE(M, name) \
  static twist::rt::sim::user::StaticVar<M> name

#define TWISTED_STATIC_MEMBER_DEFINE(T, M, name) \
  twist::rt::sim::user::StaticVar<M> T::name{#name, /*member=*/true}

#else

#include <twist/rt/thr/static/var.hpp>

#define TWISTED_STATIC_MEMBER_DECLARE(M, name) \
  static twist::rt::thr::StaticVar<M> name

#define TWISTED_STATIC_MEMBER_DEFINE(T, M, name) \
  twist::rt::thr::StaticVar<M> T::name{}

#endif
