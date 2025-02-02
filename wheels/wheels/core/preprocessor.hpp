#pragma once

// Examples:
// #define LINE 17
// _WHEELS_TO_STRING_EXPANDED(LINE) -> "LINE"
// WHEELS_TO_STRING(LINE) -> _WHEELS_TO_STRING_EXPANDED(17) -> "17"

#define _WHEELS_TO_STRING_EXPANDED(x) #x
#define WHEELS_TO_STRING(x) _WHEELS_TO_STRING_EXPANDED(x)

#define _WHEELS_CONCAT_EXPANDED(x, y) x##y
#define WHEELS_CONCAT(x, y) _WHEELS_CONCAT_EXPANDED(x, y)

#define WHEELS_UNIQUE_NAME(name) WHEELS_CONCAT(name, __COUNTER__)

#include <wheels/core/map.hpp>
