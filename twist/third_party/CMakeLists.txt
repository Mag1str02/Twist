include(FetchContent)

# --------------------------------------------------------------------

# Offline mode (uncomment next line to enable)
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)

# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

# Core components

FetchContent_Declare(
        wheels
        GIT_REPOSITORY https://gitlab.com/Lipovsky/wheels.git
        GIT_TAG master
)
FetchContent_MakeAvailable(wheels)

# --------------------------------------------------------------------

# Futex

if(NOT TWIST_SIM)
    FetchContent_Declare(
            futex_like
            GIT_REPOSITORY https://gitlab.com/Lipovsky/futex_like.git
            GIT_TAG master
    )
    FetchContent_MakeAvailable(futex_like)
endif()

# --------------------------------------------------------------------

if(TWIST_SIM)
    # Context switch
    FetchContent_Declare(
            sure
            GIT_REPOSITORY https://gitlab.com/Lipovsky/sure.git
            GIT_TAG master
    )
    FetchContent_MakeAvailable(sure)
endif()

# --------------------------------------------------------------------

# Unique Function

FetchContent_Declare(
        function2
        GIT_REPOSITORY https://github.com/Naios/function2.git
        GIT_TAG master
)
FetchContent_MakeAvailable(function2)

# --------------------------------------------------------------------

# Formatting

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

# --------------------------------------------------------------------

# Stacktrace

if(TWIST_SIM_DEBUG)
    #set(STACK_DETAILS_AUTO_DETECT ON CACHE BOOL "Auto detect backward's stack details dependencies")
    set(STACK_DETAILS_BFD TRUE CACHE BOOL "Use libbfd to read debug info")
endif()

FetchContent_Declare(
        backward
        GIT_REPOSITORY https://github.com/bombela/backward-cpp
        GIT_TAG master
)
FetchContent_MakeAvailable(backward)

# --------------------------------------------------------------------

if(TWIST_DEVELOPER OR TWIST_TESTS)
    FetchContent_Declare(
            catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.5.3
    )
    FetchContent_MakeAvailable(catch2)
endif()
