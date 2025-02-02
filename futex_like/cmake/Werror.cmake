if(FUTEX_LIKE_DEVELOPER)
    # https://clang.llvm.org/docs/DiagnosticsReference.html
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()
