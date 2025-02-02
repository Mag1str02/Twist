ProjectLog("Demo: bounded_queue")

add_executable(twist_demo_bounded_queue main.cpp)
target_link_libraries(twist_demo_bounded_queue twist)

# Demo as test

add_test(NAME twist_tests_sim_demo_bounded_queue
        COMMAND twist_demo_bounded_queue)

set_tests_properties(twist_tests_sim_demo_bounded_queue PROPERTIES LABELS "sim;demo")
