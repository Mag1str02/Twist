cmake_minimum_required(VERSION 3.9)
project(sure)

include(cmake/Logging.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

include(cmake/Sanitize.cmake)

add_subdirectory(third_party)

option(SURE_DEVELOPER "Sure development mode" OFF)
option(SURE_TESTS "Enable sure tests" OFF)

option(SURE_EXCEPTIONS "Sure exceptions context" ON)
option(SURE_UCONTEXT "Use ucontext" OFF)

include(cmake/CompileOptions.cmake)

include(cmake/Platform.cmake)
include(cmake/MachineContext.cmake)
include(cmake/ExceptionsContext.cmake)

add_subdirectory(source)

if(SURE_DEVELOPER OR SURE_TESTS)
    add_subdirectory(tests)
endif()

if(SURE_DEVELOPER)
    add_subdirectory(bench)
endif()
