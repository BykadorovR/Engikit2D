cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#need for mismatch resolving
string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
message(STATUS ${CMAKE_BINARY_DIR})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

ExternalProject_Add(
  glew
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew
  DOWNLOAD_COMMAND "" # Disable download step
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/build/cmake -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG} -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
  INSTALL_COMMAND ""
  )


ExternalProject_Add(
  glfw
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw
  DOWNLOAD_COMMAND "" # Disable download step
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -DBUILD_SHARED_LIBS=ON -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG} -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
  INSTALL_COMMAND ""
  )

ExternalProject_Add(
  libpng
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libpng
  DOWNLOAD_COMMAND "" # Disable download step
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libpng -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG} -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
  INSTALL_COMMAND ""
  )

ExternalProject_Add(
  googletest
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/googletest
  DOWNLOAD_COMMAND "" # Disable download step
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/googletest
  INSTALL_COMMAND ""
  )

#affects all projects, add folder "include" to include section in VS
include_directories("include/ECS")
include_directories("include/Graphic")
include_directories("include/GUI")

file(GLOB_RECURSE app_graphic_source "src/Graphic/*.c*")
file(GLOB_RECURSE app_ecs_source "src/ECS/*.c*")
file(GLOB_RECURSE app_gui_source "src/GUI/*.c*")
file(GLOB app_other_source "src/*.c*")
source_group("src/Graphic" FILES ${app_graphic_source})
source_group("src/ECS" FILES ${app_ecs_source})
source_group("src/GUI" FILES ${app_gui_source})
source_group("src" FILES ${app_other_source})

file(GLOB_RECURSE app_ecs_headers "include/ECS/*.h*")
file(GLOB_RECURSE app_graphic_headers "include/Graphic/*.h*")
file(GLOB_RECURSE app_gui_headers "include/GUI/*.h*")
file(GLOB app_other_headers "include/*.h*")
source_group("include/ECS" FILES ${app_ecs_headers})
source_group("include/Graphic" FILES ${app_graphic_headers})
source_group("include/GUI" FILES ${app_gui_headers})
source_group("include" FILES ${app_other_headers})

#include headers to project (so they will be shown in include folder)
add_executable(game ${app_ecs_headers} ${app_graphic_headers} ${app_gui_headers} ${app_other_headers} ${app_graphic_source} ${app_gui_source} ${app_ecs_source} ${app_other_source})

add_dependencies(game glew glfw libpng)

#add folders with gmock headers to "include" section in VS test project
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/include)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/json/include)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libpng)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/zlib)

target_link_libraries(game opengl32.lib)

#link glfw libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/glfw-prefix/src/glfw-build/src/Debug/glfw3dll.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/glfw-prefix/src/glfw-build/src/Release/glfw3dll.lib)

#link glew libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/glew-prefix/src/glew-build/lib/Debug/glew32d.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/glew-prefix/src/glew-build/lib/Release/glew32.lib)

#link libpng libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/Debug/libpng16d.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/Release/libpng16.lib)

enable_testing()

#source files
file(GLOB_RECURSE TEST_SOURCE "test/*.c*")
source_group("test" FILES ${TEST_SOURCE})

add_executable(tests ${TEST_SOURCE})

add_dependencies(tests googletest glew glfw libpng)

#add folders with gmock headers to "include" section in VS test project
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/json/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/googletest/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libpng)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/zlib)

target_link_libraries(game opengl32.lib)

#link glfw libraries
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/glfw-prefix/src/glfw-build/src/Debug/glfw3dll.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/glfw-prefix/src/glfw-build/src/Release/glfw3dll.lib)

#link glew libraries
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/glew-prefix/src/glew-build/lib/Debug/glew32d.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/glew-prefix/src/glew-build/lib/Release/glew32.lib)

#link libpng libraries
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/Debug/libpng16d.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/Release/libpng16.lib)

#link glew libraries
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/Debug/gtestd.lib)
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/Debug/gtest_maind.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/Release/gtest.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/Release/gtest_main.lib)

add_test( tests tests )