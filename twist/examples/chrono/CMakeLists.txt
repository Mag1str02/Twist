ProjectLog("Example: chrono")

add_executable(twist_example_chrono main.cpp)
target_link_libraries(twist_example_chrono twist)

add_test(NAME twist_example_chrono
        COMMAND twist_example_chrono)

set_tests_properties(twist_example_chrono PROPERTIES LABELS "example")
