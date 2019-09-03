cmake_minimum_required(VERSION 3.10)

project(onyxup)

add_subdirectory(sources src)
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests tests)
endif ()


