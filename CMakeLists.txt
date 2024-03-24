cmake_minimum_required(VERSION 3.10)
project(brctl_nl VERSION 1.0)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)

include_directories(include)

add_subdirectory(src)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_lib)
