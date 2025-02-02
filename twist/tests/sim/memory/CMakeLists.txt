add_executable(twist_tests_sim_memory
        main.cpp)

target_link_libraries(twist_tests_sim_memory PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_memory
        COMMAND twist_tests_sim_memory)

set_tests_properties(twist_tests_sim_memory PROPERTIES LABELS "sim;memory")
