ProjectLog("Example: thread_local")

add_executable(twist_example_thread_local main.cpp)
target_link_libraries(twist_example_thread_local twist)

add_test(NAME twist_example_thread_local
        COMMAND twist_example_thread_local)

set_tests_properties(twist_example_thread_local PROPERTIES LABELS "example")
