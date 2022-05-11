cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)
project(rtperfTest VERSION 0.1 DESCRIPTION "librtperflog test")

include_directories(../include)
link_directories(../ressources)
add_executable(rtperflogTest test.c)
target_link_libraries(rtperflogTest rtperflog)

