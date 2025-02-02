add_executable(twist_tests_sim_time
        main.cpp)

target_link_libraries(twist_tests_sim_time PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_time
        COMMAND twist_tests_sim_time)

set_tests_properties(twist_tests_sim_time PROPERTIES LABELS "sim;time")
