add_executable(twist_tests_sim_thread_detach
        main.cpp)

target_link_libraries(twist_tests_sim_thread_detach PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_thread_detach
        COMMAND twist_tests_sim_thread_detach)

set_tests_properties(twist_tests_sim_thread_detach PROPERTIES LABELS "sim;detach")
