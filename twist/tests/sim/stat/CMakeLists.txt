add_executable(twist_tests_sim_stat
        main.cpp)

target_link_libraries(twist_tests_sim_stat PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_stat
        COMMAND twist_tests_sim_stat)

set_tests_properties(twist_tests_sim_stat PROPERTIES LABELS "sim;stat")
