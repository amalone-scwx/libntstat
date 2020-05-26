cmake_minimum_required( VERSION 3.4 )
project(libntstat_project)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF) # want c++1x not gnu++1x

file(GLOB SRCS src/*.c*)
file(GLOB HDRS include/*.h* src/*.h*)

add_library(ntstat ${SRCS} ${HDRS})

add_executable(demo demo/main.cpp include/NetworkStatisticsClient.hpp)
target_link_libraries(demo ntstat)

add_executable(cdemo cdemo/main.c include/net_stats_client.h)
target_link_libraries(cdemo ntstat)
