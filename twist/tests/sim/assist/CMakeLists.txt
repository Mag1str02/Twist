add_executable(twist_tests_sim_assist
        main.cpp)

target_link_libraries(twist_tests_sim_assist PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_assist
        COMMAND twist_tests_sim_assist)

set_tests_properties(twist_tests_sim_assist PROPERTIES LABELS "sim;assist")
