# General

ProjectLog("CMake version: ${CMAKE_VERSION}")
ProjectLog("C++ compiler: ${CMAKE_CXX_COMPILER}")
ProjectLog("C++ standard: ${CMAKE_CXX_STANDARD}")

# Twist-ed

if(TWIST_FAULTY)
    ProjectLog("Fault injection: On")

    ProjectLog("Fault placement: ${TWIST_FAULT_PLACEMENT}")
else()
    ProjectLog("Fault injection: Off")
endif()

if(TWIST_SIM)
    ProjectLog("Runtime: Simulation")

    if(TWIST_SIM_ISOLATION)
        ProjectLog("Runtime: Isolated user memory")

        if (TWIST_SIM_FIXED_USER_MEMORY)
            ProjectLog("Runtime: Fixed user memory mapping")
        endif()
    endif()
else()
    ProjectLog("Runtime: Threads")
endif()
