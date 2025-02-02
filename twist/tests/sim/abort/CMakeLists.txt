add_executable(twist_tests_sim_abort
        main.cpp)

target_link_libraries(twist_tests_sim_abort PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_abort
        COMMAND twist_tests_sim_abort)

set_tests_properties(twist_tests_sim_abort PROPERTIES LABELS "sim;abort")
