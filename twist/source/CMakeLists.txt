# --------------------------------------------------------------------

get_filename_component(TWIST_INCLUDE_PATH "." ABSOLUTE)
get_filename_component(TWIST_SOURCE_PATH "./twist" ABSOLUTE)

# --------------------------------------------------------------------

# Sources

set(TWIST_SOURCES "${TWIST_SOURCE_PATH}/build.cpp")

CollectCxxSources("${TWIST_SOURCE_PATH}/wheels" TWIST_SOURCES)

if(TWIST_SIM)
    CollectCxxSources("${TWIST_SOURCE_PATH}/rt/sim" TWIST_SOURCES)
    CollectCxxSources("${TWIST_SOURCE_PATH}/mod/sim" TWIST_SOURCES)
else()
    CollectCxxSources("${TWIST_SOURCE_PATH}/rt/thr" TWIST_SOURCES)
    CollectCxxSources("${TWIST_SOURCE_PATH}/mod/thr" TWIST_SOURCES)
endif()

CollectCxxSources("${TWIST_SOURCE_PATH}/mod/cross" TWIST_SOURCES)

CollectCxxSources("${TWIST_SOURCE_PATH}/test" TWIST_SOURCES)
CollectCxxSources("${TWIST_SOURCE_PATH}/trace" TWIST_SOURCES)

# --------------------------------------------------------------------

# Library

add_library(twist STATIC ${TWIST_SOURCES})

target_include_directories(twist PUBLIC ${TWIST_INCLUDE_PATH})

target_link_libraries(twist PUBLIC
        wheels
        fmt
        function2
        pthread)

if(NOT TWIST_SIM)
    target_link_libraries(twist PUBLIC futex_like)
endif()

if(TWIST_SIM)
    target_link_libraries(twist PUBLIC sure)
endif()

target_link_libraries(twist PRIVATE backward)
if(TWIST_SIM_DEBUG)
    target_link_libraries(twist PRIVATE bfd dl)
endif()

# --------------------------------------------------------------------

# Compile definitions

if(TWIST_FAULTY)
    target_compile_definitions(twist PUBLIC __TWIST_FAULTY__=1)
    target_compile_definitions(twist PUBLIC __TWIST_BUILD_FAULTY__=1)

    if(TWIST_FAULT_PLACEMENT STREQUAL "BEFORE")
        target_compile_definitions(twist PUBLIC __TWIST_INJECT_FAULT_BEFORE__=1)
    elseif(TWIST_FAULT_PLACEMENT STREQUAL "AFTER")
        target_compile_definitions(twist PUBLIC __TWIST_INJECT_FAULT_AFTER__=1)
    elseif(TWIST_FAULT_PLACEMENT STREQUAL "BOTH")
        target_compile_definitions(twist PUBLIC
                __TWIST_INJECT_FAULT_BEFORE__=1
                __TWIST_INJECT_FAULT_AFTER__=1)
    else()
        message(FATAL_ERROR "Invalid fault placement")
    endif()
endif()

if(TWIST_SIM)
    target_compile_definitions(twist PUBLIC __TWIST_SIM__=1)
    target_compile_definitions(twist PUBLIC __TWIST_BUILD_SIM__=1)

    if (TWIST_SIM_ISOLATION)
        target_compile_definitions(twist PUBLIC __TWIST_SIM_ISOLATION__=1)
        target_compile_definitions(twist PUBLIC __TWIST_BUILD_SIM_ISOLATION__=1)

        if (TWIST_SIM_FIXED_USER_MEMORY)
            target_compile_definitions(twist PUBLIC __TWIST_SIM_FIXED_USER_MEMORY_MAPPING__=1)
        endif()
    endif()

    if (TWIST_SIM_DEBUG)
        target_compile_definitions(twist PUBLIC __TWIST_SIM_DEBUG__=1)
    endif()


    if (TWIST_SIM_VECTORIZE_CLOCKS)
        target_compile_definitions(twist PUBLIC __TWIST_SIM_VECTORIZE_CLOCKS__=1)
    endif()
endif()

if (NOT TWIST_SIM)
    # Threads

    target_compile_definitions(twist PUBLIC __TWIST_BUILD_THR__=1)

    ProcessorCount(ProcCount)

    if ((ProcCount EQUAL 1) OR (TWIST_FAULTY AND LINUX))
        target_compile_definitions(twist PUBLIC __TWIST_THR_SINGLE_CORE__=1)
    endif()
endif()

if (TWIST_SIM OR TWIST_FAULTY)
    target_compile_definitions(twist PUBLIC __TWISTED__=1)
    target_compile_definitions(twist PUBLIC __TWIST_BUILD_TWISTED__=1)
else()
    target_compile_definitions(twist PUBLIC __TWIST_BUILD_PLAIN__=1)
endif()

if (TWIST_ATOMIC_WAIT)
    target_compile_definitions(twist PUBLIC __TWIST_ATOMIC_WAIT__=1)
endif()

if (TWIST_NOISY_BUILD)
    target_compile_definitions(twist PUBLIC __TWIST_NOISY_BUILD__=1)
endif()
