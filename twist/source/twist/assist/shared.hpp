#pragma once

/*
 * Annotations for data race checks
 *
 * twist::assist::Shared<T>
 *
 * Usage:
 *
 * twist::assist::Shared<int> var;
 *
 * *var = 1;
 * int v = var;
 *
 * // Capture source location
 * var.Write(1);
 * int v = var.Read();
 *
 * twist::assist::Shared<Widget> w;
 *
 * w->Foo();
 * w.WriteView()->Foo();  // Capture source location
 *
 */

#include <twist/rt/cap/assist/shared.hpp>

namespace twist::assist {

using rt::cap::assist::Shared;

}  // namespace twist::assist
