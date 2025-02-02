#pragma once

#include <twist/rt/cap/static/member.hpp>

// TWISTED_STATIC_MEMBER_DECLARE(M, name)
// TWISTED_STATIC_MEMBER_DEFINE(T, M, name)

// Usage: examples/static/main.cpp

/*
 * // In .hpp
 *
 * struct Widget {
 *   void Foo();
 * };
 *
 * class Gadget {
 *  public:
 *   void Bar() {
 *     // similar to std::optional
 *     w->Foo();  // operator->
 *     (*w).Foo();  // operator*
 *   }
 *
 *  private:
 *   // ~ static Widget w;
 *   TWISTED_STATIC_MEMBER_DECLARE(Widget, w);
 * };
 *
 * // in .cpp
 *
 * // ~ Widget Gadget::w;
 * TWISTED_STATIC_MEMBER_DEFINE(Gadget, Widget, w);
 *
 */
