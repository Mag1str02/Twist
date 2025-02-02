macro(CollectCxxSources Dir OutSources)
    file(GLOB_RECURSE DirSources ${Dir}/*.cpp)
    set(${OutSources} ${${OutSources}} ${DirSources})
endmacro()
