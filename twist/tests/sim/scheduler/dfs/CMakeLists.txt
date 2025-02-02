add_executable(twist_tests_sim_scheduler_dfs
        main.cpp)

target_link_libraries(twist_tests_sim_scheduler_dfs PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_scheduler_dfs
        COMMAND twist_tests_sim_scheduler_dfs)

set_tests_properties(twist_tests_sim_scheduler_dfs PROPERTIES LABELS "sim;scheduler;dfs-scheduler")
