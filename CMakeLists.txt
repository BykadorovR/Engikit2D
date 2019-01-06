cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#need for mismatch resolving
string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

ExternalProject_Add(
  glew
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew
  DOWNLOAD_COMMAND "" # Disable download step
  #CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}"
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/build/cmake
  INSTALL_COMMAND ""
  )

ExternalProject_Add(
  freeglut
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freeglut
  DOWNLOAD_COMMAND "" # Disable download step
  #CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}"
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freeglut
  INSTALL_COMMAND ""
  )

ExternalProject_Add(
  libpng
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libpng
  DOWNLOAD_COMMAND "" # Disable download step
  #CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}"
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libpng
  INSTALL_COMMAND ""
  )

ExternalProject_Add(
  googletest
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/googletest
  DOWNLOAD_COMMAND "" # Disable download step
  #CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}"
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/googletest
  INSTALL_COMMAND ""
  )


#affects all projects, add folder "include" to include section in VS
include_directories("include")

file(GLOB_RECURSE APP_SOURCE "src/*.c*")
source_group("src" FILES ${APP_SOURCE})

file(GLOB_RECURSE APP_HEADERS "include/*.h*")
source_group("include" FILES ${APP_HEADERS})

#include headers to project (so they will be shown in include folder)
add_executable(game ${APP_HEADERS} ${APP_SOURCE})

add_dependencies(game glew freeglut libpng)

#add folders with gmock headers to "include" section in VS test project
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/include)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freeglut/include)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libpng)

#link freeglut libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/freeglut-prefix/src/freeglut-build/lib/Debug/freeglutd.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/freeglut-prefix/src/freeglut-build/lib/Release/freeglut.lib)

#link glew libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/glew-prefix/src/glew-build/lib/Debug/glew32d.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/glew-prefix/src/glew-build/lib/Release/glew32.lib)

#link libpng libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/Debug/libpng16_staticd.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/Release/libpng16_static.lib)

enable_testing()

#source files
file(GLOB_RECURSE TEST_SOURCE "test/*.c*")
source_group("test" FILES ${TEST_SOURCE})

add_executable(tests ${TEST_SOURCE})

add_dependencies(tests googletest glew freeglut libpng)

#add folders with gmock headers to "include" section in VS test project
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/googletest/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freeglut/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libpng)

#link freeglut libraries
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/freeglut-prefix/src/freeglut-build/lib/Debug/freeglutd.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/freeglut-prefix/src/freeglut-build/lib/Release/freeglut.lib)

#link glew libraries
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/glew-prefix/src/glew-build/lib/Debug/glew32d.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/glew-prefix/src/glew-build/lib/Release/glew32.lib)

#link libpng libraries
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/Debug/libpng16_staticd.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/Release/libpng16_static.lib)

#link glew libraries
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/Debug/gtestd.lib)
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/Debug/gtest_maind.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/Release/gtest.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/Release/gtest_main.lib)

add_test( tests tests )