ProjectLog("Cross unit tests: futex")

add_executable(twist_tests_cross_unit_test
        test.cpp
        main.cpp)

target_link_libraries(twist_tests_cross_unit_test PRIVATE twist)

add_test(NAME twist_tests_cross_unit_test
        COMMAND twist_tests_cross_unit_test)

set_tests_properties(twist_tests_cross_unit_test PROPERTIES LABELS "cross;unit;test")
