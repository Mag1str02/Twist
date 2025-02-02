cmake_minimum_required(VERSION 3.14)
project(twist)

include(cmake/Logging.cmake)

include(cmake/Sanitize.cmake)

option(TWIST_DEVELOPER "Twist development mode" OFF)
option(TWIST_TESTS "Enable twist tests" OFF)
option(TWIST_EXAMPLES "Enable twist examples" OFF)
option(TWIST_NOISY_BUILD "Emit warnings with runtime setup" OFF)

option(TWIST_SIM "Deterministic simulation" OFF)
option(TWIST_SIM_ISOLATION "User memory isolation in simulation" OFF)
option(TWIST_SIM_FIXED_USER_MEMORY, "Fixed user memory mapping" OFF)
option(TWIST_SIM_DEBUG "Debug libraries for backward-cpp" OFF)
option(TWIST_SIM_VECTORIZE "Vectorize vector clocks" OFF)

option(TWIST_FAULTY "Enable fault injection" OFF)
set(TWIST_FAULT_PLACEMENT "BEFORE" CACHE STRING "Where to inject faults: BEFORE (default) sync operation / AFTER / BOTH sides")

option(TWIST_ATOMIC_WAIT "Support {atomic, atomic_flag}::wait" OFF)

if(TWIST_SIM AND NOT TWIST_FAULTY)
    # ???
    message(FATAL_ERROR "Invalid Twist build configuration: simulation without fault injection.")
endif()    

if(TSAN AND TWIST_SIM)
    message(WARNING "Simulation is single threaded, so thread sanitizer just slow down execution.")
endif()

if ((TSAN OR ASAN) AND TWIST_SIM_ISOLATION)
    message(FATAL_ERROR "Invalid Twist build configuration: simulation with user memory isolation is incompatible with sanitizers.")
endif()

if (TWIST_SIM_FIXED_USER_MEMORY AND NOT TWIST_SIM_ISOLATION)
    message(FATAL_ERROR "Invalid Twist build configuration: TWIST_SIM_FIXED_USER_MEMORY=ON requires TWIST_SIM_ISOLATION=ON")
endif()

include(cmake/CompileOptions.cmake)
include(cmake/StdLibrary.cmake)

add_subdirectory(third_party)

include(cmake/Pedantic.cmake)
include(cmake/Platform.cmake)
include(cmake/Processor.cmake)
include(cmake/PrintDiagnostics.cmake)
include(cmake/CollectSources.cmake)

add_subdirectory(source)

if(TWIST_TESTS OR TWIST_DEVELOPER)
    enable_testing()
    add_subdirectory(tests)
endif()

if(TWIST_EXAMPLES OR TWIST_DEVELOPER)
    add_subdirectory(demo)
    add_subdirectory(examples)
    add_subdirectory(play)
endif()

if(TWIST_DEVELOPER)
    add_subdirectory(workloads)
endif()
