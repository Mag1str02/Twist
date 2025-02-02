add_executable(twist_tests_sim_write
        main.cpp)

target_link_libraries(twist_tests_sim_write PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_write
        COMMAND twist_tests_sim_write)

set_tests_properties(twist_tests_sim_write PROPERTIES LABELS "sim;write")
