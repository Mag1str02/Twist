ProjectLog("Example: trace")

add_executable(twist_example_trace main.cpp)
target_link_libraries(twist_example_trace twist)

add_test(NAME twist_example_trace
        COMMAND twist_example_trace)

set_tests_properties(twist_example_trace PROPERTIES LABELS "example")
