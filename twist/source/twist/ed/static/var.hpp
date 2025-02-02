#pragma once

#include <twist/rt/cap/static/var.hpp>

// static T name → TWISTED_STATIC(T, name)

// Usage: examples/static/main.cpp

/*
 * struct Widget {
 *   void Foo();
 * };
 *
 * // ~ static Widget w;
 * TWISTED_STATIC(Widget, w);
 *
 * struct Gadget {
 *   void Baz();
 * };
 *
 * void Bar() {
 *   // ~ static Gadget g;
 *   TWISTED_STATIC(Gadget, g);
 *
 *   g->Baz();
 * }
 *
 * int main() {
 *   twist::cross::Run([] {
 *     // operator-> (similar to std::optional)
 *     w->Foo();
 *
 *     // operator* (similar to std::optional)
 *     (*w).Foo();
 *
 *     Widget* w_ptr = &w;
 *     w_ptr->Foo();
 *
 *     Bar();
 *   });
 *
 *   return 0;
 * }
 *
 */
