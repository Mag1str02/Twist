ProjectLog("Cross unit tests: futex")

add_executable(twist_tests_cross_unit_futex
        futex.cpp
        main.cpp)

target_link_libraries(twist_tests_cross_unit_futex PRIVATE twist)

add_test(NAME twist_tests_cross_unit_futex
        COMMAND twist_tests_cross_unit_futex)

set_tests_properties(twist_tests_cross_unit_futex PROPERTIES LABELS "cross;unit;futex")
