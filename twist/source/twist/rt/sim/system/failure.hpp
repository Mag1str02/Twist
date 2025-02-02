#pragma once

#include <cstdint>
#include <variant>

namespace twist::rt::sim {

namespace system {

struct DataRaceContext {
  uint32_t old_access_action_index;
  uint32_t curr_access_action_index;
};

struct UseAfterFreeContext {
  uint32_t old_access_action_index;
  uint32_t curr_access_action_index;
};

struct DoubleFreeContext {
  uint32_t prev_free_action_index;
};

struct MemoryLeakContext {
  uint32_t alloc_action_index;
};

struct EmptyContext {};

using FailureContext = std::variant<  //
    EmptyContext,                     //
    DataRaceContext,                  //
    UseAfterFreeContext,              //
    DoubleFreeContext,                //
    MemoryLeakContext                 //
    >;

}  // namespace system

}  // namespace twist::rt::sim
