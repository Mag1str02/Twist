ProjectLog("Example: futex")

add_executable(twist_example_futex main.cpp)
target_link_libraries(twist_example_futex twist)

add_test(NAME twist_example_futex
        COMMAND twist_example_futex)

set_tests_properties(twist_example_futex PROPERTIES LABELS "example")
