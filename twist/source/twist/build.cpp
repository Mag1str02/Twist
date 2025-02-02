// TWIST_SIM => TWIST_FAULTY

#if (__TWIST_SIM__) && !(__TWIST_FAULTY__)

#error \
    "Invalid twist build configuration: simulation without fault injection"

#endif

// TWIST_SIM_ISOLATION xor {Address,Thread}Sanitizer

#if defined(__TWIST_SIM_ISOLATION__)

#if defined(__has_feature)
#if __has_feature(address_sanitizer) || __has_feature(thread_sanitizer)

#error \
    "Invalid twist build configuration: Isolated simulation & {Address,Thread}Sanitizer"

#endif
#endif

#endif

#if defined(__TWIST_NOISY_BUILD__)

#if defined(__TWIST_SIM__)
#pragma message("Simulation")
#else
#pragma message("Threads")
#endif

#if defined(__TWIST_FAULTY__)
#pragma message("Fault injection enabled")
#endif

#if defined(__TWIST_THR_SINGLE_CORE__)
#pragma message("Single-core mode")
#else
#pragma message("Multi-core mode")
#endif

#endif
