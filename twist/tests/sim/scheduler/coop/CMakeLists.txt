add_executable(twist_tests_sim_scheduler_coop
        main.cpp)

target_link_libraries(twist_tests_sim_scheduler_coop PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_scheduler_coop
        COMMAND twist_tests_sim_scheduler_coop)

set_tests_properties(twist_tests_sim_scheduler_coop PROPERTIES LABELS "sim;scheduler;coop-scheduler")
