# Stacktrace

if(TWIST_SIM_DEBUG)
    #set(STACK_DETAILS_AUTO_DETECT ON CACHE BOOL "Auto detect backward's stack details dependencies")
    set(STACK_DETAILS_BFD TRUE CACHE BOOL "Use libbfd to read debug info")
endif()

# --------------------------------------------------------------------

if(TWIST_DEVELOPER OR TWIST_TESTS)
    FetchContent_Declare(
            catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.5.3
    )
    FetchContent_MakeAvailable(catch2)
endif()
