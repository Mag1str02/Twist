add_compile_options(-Wall -Wextra -Wpedantic)

if(TWIST_DEVELOPER)
    add_compile_options(-Werror)
endif()
