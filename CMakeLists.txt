cmake_minimum_required(VERSION 3.1...3.15)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project("x80806_64" VERSION 0.1 LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Release)

add_library(intel8080 STATIC cpu/cpu.cpp cpu/cpu.h)

add_executable(test8080 test/main.cpp test/utils.cpp test/utils.h)
target_link_libraries(test8080 PRIVATE intel8080)