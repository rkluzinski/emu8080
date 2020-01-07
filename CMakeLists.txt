cmake_minimum_required(VERSION 3.1...3.15)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
project("x80806_64" VERSION 0.1 LANGUAGES CXX)

add_library(intel8080 STATIC cpu/cpu.cpp cpu/cpu.h)
target_compile_options(intel8080 PUBLIC
    -std=c++17
    -Wall -Wextra -Werror
    -Ofast -march=native)

add_executable(test-runner test/main.cpp)
target_link_libraries(test-runner PRIVATE intel8080)