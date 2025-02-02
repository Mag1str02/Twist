#pragma once

#include <wheels/core/preprocessor.hpp>

/*
 * // Static var visitor with unique name
 *
 * TWISTED_STATIC_VAR(Widget, w) expands to
 *
 * static Widget w;
 *
 * struct StaticVar11wVisitor {
 *   StaticVar11wVisitor() {
 *     w._Visit();
 *   }
 * } _visit_w_12
 *
 */

#define ___TWIST_VISIT_STATIC_VAR_IMPL_EXPANDED(name, id) \
  struct StaticVar##id##name##Visitor {             \
    StaticVar##id##name##Visitor() {                \
      name._Visit();                                               \
    }                                                 \
  } WHEELS_UNIQUE_NAME(_visit_##name##_)

#define ___TWIST_VISIT_STATIC_VAR_IMPL(name, id) \
  ___TWIST_VISIT_STATIC_VAR_IMPL_EXPANDED(name, id)

#define ___TWIST_VISIT_STATIC_VAR(name) \
  ___TWIST_VISIT_STATIC_VAR_IMPL(name, __COUNTER__)
