ProjectLog("Example: assist")

add_executable(twist_example_assist main.cpp)
target_link_libraries(twist_example_assist twist)

add_test(NAME twist_example_assist
        COMMAND twist_example_assist)

set_tests_properties(twist_example_assist PROPERTIES LABELS "example")
