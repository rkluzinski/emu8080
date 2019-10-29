cmake_minimum_required(VERSION 3.1...3.15)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project("x80806_64" VERSION 0.1 LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Debug)

add_library(cpu8080 STATIC src/cpu.cpp src/cpu.h)

add_executable(cpu_test test/test.cpp)
target_link_libraries(cpu_test PRIVATE cpu8080)