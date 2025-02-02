ProjectLog("MachineContext: ${SURE_MACHINE_CONTEXT}")
ProjectLog("ExceptionsContext: ${SURE_EXCEPTIONS_CONTEXT}")

get_filename_component(SURE_INCLUDE_PATH "." ABSOLUTE)
get_filename_component(SURE_SOURCE_PATH "./sure" ABSOLUTE)

set(SURE_SOURCES
        ${SURE_SOURCE_PATH}/context.cpp
        ${SURE_SOURCE_PATH}/stack/mmap.cpp)

# Machine context

set(SURE_MACHINE_CONTEXT_SOURCES
        ${SURE_SOURCE_PATH}/machine/${SURE_MACHINE_CONTEXT}/context.cpp
        ${SURE_SOURCE_PATH}/machine/${SURE_MACHINE_CONTEXT}/context.S)

# Exceptions context

set(SURE_EXCEPTIONS_CONTEXT_SOURCES
        ${SURE_SOURCE_PATH}/exceptions/${SURE_EXCEPTIONS_CONTEXT}/context.cpp)

enable_language(ASM)

add_library(sure STATIC
        ${SURE_SOURCES}
        ${SURE_MACHINE_CONTEXT_SOURCES}
        ${SURE_EXCEPTIONS_CONTEXT_SOURCES})

target_include_directories(sure PUBLIC ${SURE_INCLUDE_PATH})

target_link_libraries(sure PUBLIC wheels)

target_compile_definitions(sure PUBLIC SURE_MACHINE_CONTEXT_${SURE_MACHINE_CONTEXT})
target_compile_definitions(sure PUBLIC SURE_EXCEPTIONS_CONTEXT_${SURE_EXCEPTIONS_CONTEXT})
