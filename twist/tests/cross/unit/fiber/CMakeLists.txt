add_compile_options(-Wno-error=unused-command-line-argument)

include(FetchContent)

FetchContent_Declare(
        sure
        GIT_REPOSITORY https://gitlab.com/Lipovsky/sure.git
        GIT_TAG master
)
FetchContent_MakeAvailable(sure)

###

ProjectLog("Cross unit tests: fiber")

add_executable(twist_tests_cross_unit_fiber
        coroutine.cpp
        fiber.cpp
        main.cpp)

target_link_libraries(twist_tests_cross_unit_fiber PRIVATE twist sure)

add_test(NAME twist_tests_cross_unit_fiber
        COMMAND twist_tests_cross_unit_fiber)

set_tests_properties(twist_tests_cross_unit_fiber PROPERTIES LABELS "cross;unit;fiber")
