ProjectLog("Tests")

# Coroutine

add_executable(sure_tests_coroutine coroutine.cpp)
target_link_libraries(sure_tests_coroutine PRIVATE sure Catch2::Catch2WithMain)

add_test(NAME sure_tests_coroutine COMMAND sure_tests_coroutine)
set_tests_properties(sure_tests_coroutine PROPERTIES LABELS "sure;switch")

# Stacks

add_executable(sure_tests_stacks mmap_stack.cpp new_stack.cpp)
target_link_libraries(sure_tests_stacks PRIVATE sure Catch2::Catch2WithMain)

add_test(NAME sure_tests_stacks COMMAND sure_tests_stacks)
set_tests_properties(sure_tests_stacks PROPERTIES LABELS "sure;stack")
