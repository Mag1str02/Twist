#pragma once

#include <wheels/core/string_builder.hpp>
#include <wheels/core/source_location.hpp>

// Simple asynchronous logging

namespace wheels {

//////////////////////////////////////////////////////////////////////

enum class LogLevel : int {
  Trace = 1,
  Debug = 2,
  Info = 3,
  Warning = 4,
  Critical = 5
};

//////////////////////////////////////////////////////////////////////

bool LevelAccepted(LogLevel level);

void LogMessage(SourceLocation where, std::string message);

void FlushPendingLogMessages();

//////////////////////////////////////////////////////////////////////

void LogMessageSimple(std::string message);

//////////////////////////////////////////////////////////////////////

#define _LOG_IMPL(level, expr)                                          \
  do {                                                                  \
    if (wheels::LevelAccepted(level)) {                                 \
      wheels::LogMessage(WHEELS_HERE, wheels::StringBuilder() << expr); \
    }                                                                   \
  } while (false);

#define LOG_DEBUG(expr) _LOG_IMPL(wheels::LogLevel::Debug, expr)
#define LOG_TRACE(expr) _LOG_IMPL(wheels::LogLevel::Trace, expr)
#define LOG_INFO(expr) _LOG_IMPL(wheels::LogLevel::Info, expr)

}  // namespace wheels
