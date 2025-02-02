ProjectLog("Tests")

# List sources

file(GLOB_RECURSE TEST_SOURCES "./*.cpp")

# All tests target

add_executable(wheels_all_tests ${TEST_SOURCES})
target_link_libraries(wheels_all_tests wheels)
