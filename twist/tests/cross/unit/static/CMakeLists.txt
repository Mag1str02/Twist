ProjectLog("Cross unit tests: static")

add_executable(twist_tests_cross_unit_static
        main.cpp)

target_link_libraries(twist_tests_cross_unit_static PRIVATE twist)

add_test(NAME twist_tests_cross_unit_static
        COMMAND twist_tests_cross_unit_static)

set_tests_properties(twist_tests_cross_unit_static PROPERTIES LABELS "cross;unit;static")
