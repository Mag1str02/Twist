ProjectLog("Cross unit tests: memory")

add_executable(twist_tests_cross_unit_memory
        new.cpp
        malloc.cpp
        main.cpp)

target_link_libraries(twist_tests_cross_unit_memory PRIVATE twist)

add_test(NAME twist_tests_cross_unit_memory
        COMMAND twist_tests_cross_unit_memory)

set_tests_properties(twist_tests_cross_unit_memory PROPERTIES LABELS "cross;unit;memory")
