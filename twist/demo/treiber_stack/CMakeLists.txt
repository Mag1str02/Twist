ProjectLog("Demo: treiber_stack")

add_executable(twist_demo_treiber_stack main.cpp)
target_link_libraries(twist_demo_treiber_stack twist)

# Demo as test

add_test(NAME twist_tests_sim_demo_treiber_stack
        COMMAND twist_demo_treiber_stack)

set_tests_properties(twist_tests_sim_demo_treiber_stack PROPERTIES LABELS "sim;demo")
