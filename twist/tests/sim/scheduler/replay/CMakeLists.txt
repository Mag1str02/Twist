add_executable(twist_tests_sim_scheduler_replay
        main.cpp)

target_link_libraries(twist_tests_sim_scheduler_replay PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_scheduler_replay
        COMMAND twist_tests_sim_scheduler_replay)

set_tests_properties(twist_tests_sim_scheduler_replay PROPERTIES LABELS "sim;scheduler;replay-scheduler")
