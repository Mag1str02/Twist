#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/static/var.hpp>
#include <twist/rt/sim/user/static/visit.hpp>

#define TWISTED_STATIC_VAR(T, name) \
  static twist::rt::sim::user::StaticVar<T> name{#name}; \
  ___TWIST_VISIT_STATIC_VAR(name)

#else

#include <twist/rt/thr/static/var.hpp>

#define TWISTED_STATIC_VAR(T, name) static twist::rt::thr::StaticVar<T> name{}

#endif

#define TWISTED_STATIC(T, name) TWISTED_STATIC_VAR(T, name)
