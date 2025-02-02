

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
