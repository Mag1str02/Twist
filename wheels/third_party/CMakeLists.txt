include(FetchContent)

# --------------------------------------------------------------------

# Offline mode
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)
# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

FetchContent_Declare(gflags
        GIT_REPOSITORY	https://github.com/gflags/gflags.git
        GIT_TAG			master
)

FetchContent_MakeAvailable(gflags)
