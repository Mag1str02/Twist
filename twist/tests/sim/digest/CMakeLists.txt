add_executable(twist_tests_sim_digest
        main.cpp)

target_link_libraries(twist_tests_sim_digest PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_digest
        COMMAND twist_tests_sim_digest)

set_tests_properties(twist_tests_sim_digest PROPERTIES LABELS "sim;digest")
