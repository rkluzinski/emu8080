cmake_minimum_required(VERSION 3.1...3.15)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project("x80806_64" VERSION 0.1 LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(intel8080 STATIC src/cpu.cpp src/cpu.h)

add_executable(hello test/hello.cpp test/utils.cpp test/utils.h)
target_link_libraries(hello PRIVATE intel8080)

add_executable(tst8080 test/tst8080.cpp test/utils.cpp test/utils.h)
target_link_libraries(tst8080 PRIVATE intel8080)