ProjectLog("Cross unit tests: spin")

add_executable(twist_tests_cross_unit_spin
        spin.cpp
        main.cpp)

target_link_libraries(twist_tests_cross_unit_spin PRIVATE twist)

add_test(NAME twist_tests_cross_unit_spin
        COMMAND twist_tests_cross_unit_spin)

set_tests_properties(twist_tests_cross_unit_spin PROPERTIES LABELS "cross;unit;spin")
