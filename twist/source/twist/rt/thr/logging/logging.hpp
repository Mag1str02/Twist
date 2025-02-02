#pragma once

#include <twist/trace/scope.hpp>

#include <wheels/core/source_location.hpp>

#include <string_view>

// Simple asynchronous logging

namespace twist::rt::thr::log {

void LogMessage(trace::Scope* scope, wheels::SourceLocation where, std::string_view message);

void FlushPendingLogMessages();

}  // namespace twist::rt::thr::log
