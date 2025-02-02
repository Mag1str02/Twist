ProjectLog("Example: spinlock")

add_executable(twist_example_spin main.cpp)
target_link_libraries(twist_example_spin twist)

add_test(NAME twist_example_spin
        COMMAND twist_example_spin)

set_tests_properties(twist_example_spin PROPERTIES LABELS "example")
