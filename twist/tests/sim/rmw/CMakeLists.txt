add_executable(twist_tests_sim_rmw
        main.cpp)

target_link_libraries(twist_tests_sim_rmw PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_rmw
        COMMAND twist_tests_sim_rmw)

set_tests_properties(twist_tests_sim_rmw PROPERTIES LABELS "sim;rmw")
