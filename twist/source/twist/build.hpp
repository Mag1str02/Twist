#pragma once

namespace twist::build {

constexpr bool Sim() {
#if defined(__TWIST_SIM__) && defined(__TWIST_FAULTY__)
  return true;
#else
  return false;
#endif
}

constexpr bool IsolatedSim() {
#if defined(__TWIST_SIM__) && defined(__TWIST_SIM_ISOLATION__) && defined(__TWIST_FAULTY__)
  return true;
#else
  return false;
#endif
}

constexpr bool Thr() {
#if defined(__TWIST_SIM__)
  return false;
#else
  return true;
#endif
}

constexpr bool Faulty() {
#if defined(__TWIST_FAULTY__)
  return true;
#else
  return false;
#endif
}

constexpr bool Plain() {
#if !defined(__TWIST_FAULTY__) && !defined(__TWIST_SIM__)
  return true;
#else
  return false;
#endif
}

// Flags

static constexpr bool kSim = Sim();
static constexpr bool kIsolatedSim = IsolatedSim();

static constexpr bool kThr = Thr();

static constexpr bool kPlain = Plain();
static constexpr bool kTwisted = !Plain();

}  // namespace twist::build
