cmake_minimum_required(VERSION 3.0.0)
project(temp VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

find_package(fmt CONFIG REQUIRED)
find_package(Boost)

add_compile_options(-std=c++2b -Wall -Wextra -pedantic)

add_executable(temp main.cpp)

include_directories(temp ${Boost_INCLUDE_DIR})

target_link_libraries(temp PRIVATE fmt::fmt-header-only)
target_link_libraries(temp PRIVATE ws2_32)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
