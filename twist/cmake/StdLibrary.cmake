if(TWIST_DEVELOPER)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # libc++
        add_compile_options(-stdlib=libc++)
        add_link_options(-stdlib=libc++)
    endif()
endif()
