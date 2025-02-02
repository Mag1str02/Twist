cmake_minimum_required(VERSION 3.14)
project(wheels)

include(cmake/Logging.cmake)

option(WHEELS_TESTS "Enable wheels tests" OFF)
option(WHEELS_DEVELOPER "Wheels development mode" OFF)

add_subdirectory(third_party)

include(cmake/CompileOptions.cmake)
include(cmake/Platform.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

add_subdirectory(wheels)

if(WHEELS_TESTS OR WHEELS_DEVELOPER)
    add_subdirectory(tests)
endif()
