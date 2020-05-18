cmake_minimum_required(VERSION 3.1...3.15)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
project("Emu8080" VERSION 0.1 LANGUAGES CXX)

# Build the library
add_library(emu8080 STATIC src/cpu.cpp src/cpu.h)
target_compile_options(emu8080 PUBLIC
    -Wall -Wextra -Werror
    -Ofast -march=native)

# Build the test harness executable
add_executable(test-runner test/main.cpp)
target_link_libraries(test-runner PRIVATE emu8080)

# add_executable(space-invaders test/main.cpp)
# target_link_libraries(space-invaders PRIVATE intel8080)