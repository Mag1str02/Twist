add_executable(twist_tests_sim_happens_before
        main.cpp)

target_link_libraries(twist_tests_sim_happens_before PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_happens_before
        COMMAND twist_tests_sim_happens_before)

set_tests_properties(twist_tests_sim_happens_before PROPERTIES LABELS "sim;happens-before;coop-scheduler")
