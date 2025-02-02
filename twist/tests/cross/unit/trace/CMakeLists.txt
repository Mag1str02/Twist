ProjectLog("Cross unit tests: tracing")

add_executable(twist_tests_cross_unit_trace
        trace.cpp
        main.cpp)

target_link_libraries(twist_tests_cross_unit_trace PRIVATE twist)

add_test(NAME twist_tests_cross_unit_trace
        COMMAND twist_tests_cross_unit_trace)

set_tests_properties(twist_tests_cross_unit_trace PROPERTIES LABELS "cross;unit;trace")
