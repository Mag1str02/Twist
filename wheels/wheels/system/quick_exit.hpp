#pragma once

namespace wheels {

// Best effort: std::quick_exit on Linux, std::exit on OSX
void QuickExit(int exit_code);

}  // namespace wheels
