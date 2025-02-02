ProjectLog("Demo: mutex_lock_order")

add_executable(twist_demo_mutex_lock_order main.cpp)
target_link_libraries(twist_demo_mutex_lock_order twist)

# Demo as test

add_test(NAME twist_tests_sim_demo_mutex_lock_order
        COMMAND twist_demo_mutex_lock_order)

set_tests_properties(twist_tests_sim_demo_mutex_lock_order PROPERTIES LABELS "sim;demo")
