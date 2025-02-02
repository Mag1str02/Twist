ProjectLog("Example: static")

add_executable(twist_example_static main.cpp)
target_link_libraries(twist_example_static twist)

add_test(NAME twist_example_static
        COMMAND twist_example_static)

set_tests_properties(twist_example_static PROPERTIES LABELS "example")
