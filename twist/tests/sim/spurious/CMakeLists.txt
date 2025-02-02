add_executable(twist_tests_sim_spurious
        main.cpp)

target_link_libraries(twist_tests_sim_spurious PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_spurious
        COMMAND twist_tests_sim_spurious)

set_tests_properties(twist_tests_sim_spurious PROPERTIES LABELS "sim;spurious")
