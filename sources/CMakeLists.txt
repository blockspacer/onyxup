cmake_minimum_required(VERSION 3.10)
project(onyxup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O2")

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(FATAL_ERROR "BUILD ONLY LINUX !!!")
endif()

add_library(onyxup SHARED
        buffer/buffer.cpp
        httpparser/picohttpparser.cpp
        mime/types.cpp
        request/request.cpp
        response/response-base.cpp
        server/server.cpp
        task/task.cpp
        services/statistics/StatisticsService.cpp
        server/utils.cpp)

if (BUILD_DEBUG_MODE)
    add_definitions(-DDEBUG_MODE)
endif ()

target_link_libraries(onyxup pthread z)
set_property(TARGET onyxup PROPERTY POSITION_INDEPENDENT_CODE ON)

install(TARGETS onyxup DESTINATION /usr/lib/)
install(DIRECTORY ../sources/ DESTINATION /usr/include/onyxup FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ../sources/ DESTINATION /usr/include/onyxup FILES_MATCHING PATTERN "*.hpp")
install(CODE "execute_process(COMMAND ldconfig)")

