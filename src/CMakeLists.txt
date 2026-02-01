cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXECUTABLE_SUFFIX ${EXECUTABLE_SUFFIX})

project(wilimon VERSION 1.0.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME} main.cpp)
# add_executable(${PROJECT_NAME} stepcount.cpp)
# add_executable(${PROJECT_NAME} battle.cpp)