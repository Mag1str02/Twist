add_executable(twist_tests_sim_abort_static
        main.cpp)

target_link_libraries(twist_tests_sim_abort_static PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_abort_static
        COMMAND twist_tests_sim_abort_static)

set_tests_properties(twist_tests_sim_abort_static PROPERTIES LABELS "sim;abort")
