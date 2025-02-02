ProjectLog("Cross unit tests: random")

add_executable(twist_tests_cross_unit_random
        random.cpp
        main.cpp)

target_link_libraries(twist_tests_cross_unit_random PRIVATE twist)

add_test(NAME twist_tests_cross_unit_random
        COMMAND twist_tests_cross_unit_random)

set_tests_properties(twist_tests_cross_unit_random PROPERTIES LABELS "cross;unit;random")
