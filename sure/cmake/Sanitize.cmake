# Address Sanitizer
# https://clang.llvm.org/docs/AddressSanitizer.html

set(ASAN_COMPILE_FLAGS -fsanitize=address,undefined -fno-sanitize-recover=all)
set(ASAN_LINK_FLAGS -fsanitize=address,undefined)

# Thread Sanitizer
# https://clang.llvm.org/docs/ThreadSanitizer.html

set(TSAN_COMPILE_FLAGS -fsanitize=thread -fno-sanitize-recover=all)
set(TSAN_LINK_FLAGS -fsanitize=thread)

if(ASAN)
    ProjectLog("Sanitize with Address Sanitizer")
    add_compile_options(${ASAN_COMPILE_FLAGS})
    add_link_options(${ASAN_LINK_FLAGS})
endif()

if(TSAN)
    ProjectLog("Sanitize with Thread Sanitizer")
    add_compile_options(${TSAN_COMPILE_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${TSAN_LINK_FLAGS}")
endif()
