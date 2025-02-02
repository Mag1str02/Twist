cmake_minimum_required(VERSION 3.20)

project(futex_like)

option(FUTEX_LIKE_DEVELOPER "futex_like development mode" OFF)
option(FUTEX_LIKE_TESTS "Enable futex_like tests" OFF)
option(FUTEX_LIKE_EXAMPLES "Enable futex_like examples" OFF)

include(cmake/Logging.cmake)
include(cmake/Cxx.cmake)

include(cmake/Werror.cmake)

add_subdirectory(source)

if(FUTEX_LIKE_DEVELOPER OR FUTEX_LIKE_EXAMPLES)
  add_subdirectory(examples)
endif()

if(FUTEX_LIKE_DEVELOPER OR FUTEX_LIKE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
