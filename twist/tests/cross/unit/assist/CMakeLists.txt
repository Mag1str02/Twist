ProjectLog("Cross unit tests: assist")

add_executable(twist_tests_cross_unit_assist
        assist.cpp
        main.cpp)

target_link_libraries(twist_tests_cross_unit_assist PRIVATE twist)

add_test(NAME twist_tests_cross_unit_assist
        COMMAND twist_tests_cross_unit_assist)

set_tests_properties(twist_tests_cross_unit_assist PROPERTIES LABELS "cross;unit;assist")
