ProjectLog("Cross unit tests: std")

add_executable(twist_tests_cross_unit_std
        main.cpp
        atomic.cpp
        atomic_wait.cpp
        thread.cpp
        mutex.cpp
        condition_variable.cpp
        shared_mutex.cpp
        lock_guard.cpp
        unique_lock.cpp
        chrono.cpp
        random.cpp)

target_link_libraries(twist_tests_cross_unit_std PRIVATE twist)

add_test(NAME twist_tests_cross_unit_std
        COMMAND twist_tests_cross_unit_std)

set_tests_properties(twist_tests_cross_unit_std PROPERTIES LABELS "cross;unit;std")
