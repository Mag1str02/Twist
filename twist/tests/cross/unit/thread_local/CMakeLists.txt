ProjectLog("Cross unit tests: thread_local")

add_executable(twist_tests_cross_unit_thread_local
        main.cpp)

target_link_libraries(twist_tests_cross_unit_thread_local PRIVATE twist)

add_test(NAME twist_tests_cross_unit_thread_local
        COMMAND twist_tests_cross_unit_thread_local)

set_tests_properties(twist_tests_cross_unit_thread_local PROPERTIES LABELS "cross;unit;thread_local")
