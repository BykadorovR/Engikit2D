cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#need for mismatch resolving
STRING(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#affects all projects, add folder "include" to include section in VS
include_directories("include")

FILE(GLOB_RECURSE APP_SOURCE "src/*.c*")
source_group("src" FILES ${APP_SOURCE})

FILE(GLOB_RECURSE APP_HEADERS "include/*.h*")
source_group("include" FILES ${APP_HEADERS})

#include headers to project (so they will be shown in include folder)
add_executable(game ${APP_HEADERS} ${APP_SOURCE})

ADD_SUBDIRECTORY ("dependencies/googletest")
enable_testing()

#source files
FILE(GLOB_RECURSE TEST_SOURCE "test/*.c*")
source_group("test" FILES ${TEST_SOURCE})

add_executable(tests ${TEST_SOURCE})

#add folders with gmock headers to "include" section in VS test project
target_include_directories(tests PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

target_link_libraries(tests gtest gtest_main)
add_test( tests tests )