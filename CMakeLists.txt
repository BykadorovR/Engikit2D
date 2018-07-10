cmake_minimum_required(VERSION 3.7)
project(game)

#source files
FILE(GLOB_RECURSE APP_SOURCE "src/*.c*")
source_group("src" FILES ${APP_SOURCE})

#include headers
include_directories(include)
FILE(GLOB_RECURSE APP_HEADERS "include/*.h*")
source_group("include" FILES ${APP_HEADERS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(${PROJECT_NAME} ${APP_HEADERS} ${APP_SOURCE})