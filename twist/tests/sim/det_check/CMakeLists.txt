add_executable(twist_tests_sim_det_check
        main.cpp)

target_link_libraries(twist_tests_sim_det_check PRIVATE twist Catch2::Catch2WithMain)

add_test(NAME twist_tests_sim_det_check
        COMMAND twist_tests_sim_det_check)

set_tests_properties(twist_tests_sim_det_check PROPERTIES LABELS "sim;det_check")
