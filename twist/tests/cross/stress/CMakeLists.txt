ProjectLog("Cross stress tests")

# ---

add_executable(twist_tests_cross_stress_atomic_flag atomic_flag.cpp)
target_link_libraries(twist_tests_cross_stress_atomic_flag PRIVATE twist)

add_test(NAME twist_tests_cross_stress_atomic_flag
        COMMAND twist_tests_cross_stress_atomic_flag)

set_tests_properties(twist_tests_cross_stress_atomic_flag PROPERTIES LABELS "cross;stress")

# ---

add_executable(twist_tests_cross_stress_futex futex.cpp)
target_link_libraries(twist_tests_cross_stress_futex PRIVATE twist)

add_test(NAME twist_tests_cross_stress_futex
        COMMAND twist_tests_cross_stress_futex)

set_tests_properties(twist_tests_cross_stress_futex PROPERTIES LABELS "cross;stress")

# ---

add_executable(twist_tests_cross_stress_sleep_for sleep_for.cpp)
target_link_libraries(twist_tests_cross_stress_sleep_for PRIVATE twist)

add_test(NAME twist_tests_cross_stress_sleep_for
        COMMAND twist_tests_cross_stress_sleep_for)

set_tests_properties(twist_tests_cross_stress_sleep_for PROPERTIES LABELS "cross;stress")
