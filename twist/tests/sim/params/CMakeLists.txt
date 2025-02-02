add_executable(twist_tests_sim_params
        main.cpp)

target_link_libraries(twist_tests_sim_params PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_params
        COMMAND twist_tests_sim_params)

set_tests_properties(twist_tests_sim_params PROPERTIES LABELS "sim;params")
