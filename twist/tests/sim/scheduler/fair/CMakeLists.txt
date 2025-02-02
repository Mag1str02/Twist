add_executable(twist_tests_sim_scheduler_fair
        main.cpp)

target_link_libraries(twist_tests_sim_scheduler_fair PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_scheduler_fair
        COMMAND twist_tests_sim_scheduler_fair)

set_tests_properties(twist_tests_sim_scheduler_fair PROPERTIES LABELS "sim;scheduler;fair-scheduler")
