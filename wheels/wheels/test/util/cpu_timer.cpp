#include "cpu_timer.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#include <windows.h>
#endif

namespace wheels {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
double ProcessCPUTimer::GetTimeInSeconds() const {
  FILETIME create_time;
  FILETIME exit_time;
  FILETIME kernel_time;
  FILETIME user_time;

  auto code = GetProcessTimes(GetCurrentProcess(), &create_time, &exit_time,
                              &kernel_time, &user_time);
  assert(code != -1);

  SYSTEMTIME user_system_time;
  code = FileTimeToSystemTime(&user_time, &user_system_time);
  assert(code != -1);

  return (double)user_system_time.wHour * 3600.0 +
         (double)user_system_time.wMinute * 60.0 +
         (double)user_system_time.wSecond +
         (double)user_system_time.wMilliseconds / 1000.0;
}
#else
double ProcessCPUTimer::GetTimeInSeconds() const {
  return double(std::clock()) / CLOCKS_PER_SEC;
}
#endif
}  // namespace wheels
