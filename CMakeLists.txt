cmake_minimum_required(VERSION 3.9)

project(bulk_server LANGUAGES CXX)

find_package(Boost COMPONENTS system REQUIRED)
find_package(Threads)
include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(bulk_server main.cpp bulk.cpp bulk.h logger.cpp logger.h async_server.h)

#add_definitions("-DCOMMON_STATIC_BULK")

target_link_libraries(bulk_server
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )
