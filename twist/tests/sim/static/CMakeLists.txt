add_executable(twist_tests_sim_static
        main.cpp)

target_link_libraries(twist_tests_sim_static PRIVATE twist)

add_test(NAME twist_tests_sim_static
        COMMAND twist_tests_sim_static)

set_tests_properties(twist_tests_sim_static PROPERTIES LABELS "sim;static")
