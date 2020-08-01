cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()
#need for mismatch resolving
string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
message(STATUS ${CMAKE_BINARY_DIR})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

ExternalProject_Add(
  assimp
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp
  DOWNLOAD_COMMAND "" # Disable download step
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG} -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
  INSTALL_COMMAND ""
  )

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
  freetype
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freetype
  DOWNLOAD_COMMAND "" # Disable download step
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -DBUILD_SHARED_LIBS=ON -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freetype -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG} -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
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
include_directories("include/Interaction")
include_directories("include/Network")
include_directories("include/Sound")
include_directories("include/UI")
include_directories("include/UserInput")
include_directories("include/Utility")
include_directories("include/Engine")

file(GLOB_RECURSE app_source_ecs "src/ECS/*.c*")
source_group("ECS\\src" FILES ${app_source_ecs})
file(GLOB_RECURSE app_source_graphic "src/Graphic/*.c*")
source_group("Graphic\\src" FILES ${app_source_graphic})
file(GLOB_RECURSE app_source_interaction "src/Interaction/*.c*")
source_group("Interaction\\src" FILES ${app_source_interaction})
file(GLOB_RECURSE app_source_network "src/Network/*.c*")
source_group("Network\\src" FILES ${app_source_network})
file(GLOB_RECURSE app_source_sound "src/Sound/*.c*")
source_group("Sound\\src" FILES ${app_source_sound})
file(GLOB_RECURSE app_source_ui "src/UI/*.c*")
source_group("UI\\src" FILES ${app_source_ui})
file(GLOB_RECURSE app_source_user "src/UserInput/*.c*")
source_group("UserInput\\src" FILES ${app_source_user})
file(GLOB_RECURSE app_source_utility "src/Utility/*.c*")
source_group("Utility\\src" FILES ${app_source_utility})
file(GLOB_RECURSE app_source_engine "src/Engine/*.c*")
source_group("Engine\\src" FILES ${app_source_engine})

file(GLOB_RECURSE app_header_ecs "include/ECS/*.h*")
source_group("ECS\\include" FILES ${app_header_ecs})
file(GLOB_RECURSE app_header_graphic "include/Graphic/*.h*")
source_group("Graphic\\include" FILES ${app_header_graphic})
file(GLOB_RECURSE app_header_interaction "include/Interaction/*.h*")
source_group("Interaction\\include" FILES ${app_header_interaction})
file(GLOB_RECURSE app_header_network "include/Network/*.h*")
source_group("Network\\include" FILES ${app_header_network})
file(GLOB_RECURSE app_header_sound "include/Sound/*.h*")
source_group("Sound\\include" FILES ${app_header_sound})
file(GLOB_RECURSE app_header_ui "include/UI/*.h*")
source_group("UI\\include" FILES ${app_header_ui})
file(GLOB_RECURSE app_header_user "include/UserInput/*.h*")
source_group("UserInput\\include" FILES ${app_header_user})
file(GLOB_RECURSE app_header_utility "include/Utility/*.h*")
source_group("Utility\\include" FILES ${app_header_utility})
file(GLOB_RECURSE app_header_engine "include/Engine/*.h*")
source_group("Engine\\include" FILES ${app_header_engine})

#include headers to project (so they will be shown in include folder)
add_executable(game ${app_header_ecs} ${app_source_ecs}
					${app_header_graphic} ${app_source_graphic}
					${app_header_interaction} ${app_source_interaction}
					${app_header_network} ${app_source_network}
					${app_header_sound} ${app_source_sound}
					${app_header_ui} ${app_source_ui}
					${app_header_user} ${app_source_user}
					${app_header_utility} ${app_source_utility}
					${app_header_engine} ${app_source_engine})

add_dependencies(game glew glfw libpng freetype assimp)

#add folders with gmock headers to "include" section in VS test project
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/include)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/json/include)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freetype/include)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libpng)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/zlib)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/include)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/assimp-prefix/src/assimp-build/include)

target_link_libraries(game opengl32.lib)

#link freetype libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/freetype-prefix/src/freetype-build/Debug/freetyped.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/freetype-prefix/src/freetype-build/Release/freetype.lib)

#link glfw libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/glfw-prefix/src/glfw-build/src/Debug/glfw3dll.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/glfw-prefix/src/glfw-build/src/Release/glfw3dll.lib)

#link glew libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/glew-prefix/src/glew-build/lib/Debug/glew32d.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/glew-prefix/src/glew-build/lib/Release/glew32.lib)

#link libpng libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/Debug/libpng16d.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/Release/libpng16.lib)

#link libpng libraries
target_link_libraries(game debug ${CMAKE_CURRENT_BINARY_DIR}/assimp-prefix/src/assimp-build/code/Debug/assimp-vc141-mtd.lib)
target_link_libraries(game optimized ${CMAKE_CURRENT_BINARY_DIR}/assimp-prefix/src/assimp-build/code/Release/assimp-vc141-mt.lib)

enable_testing()

#source files
file(GLOB_RECURSE TEST_SOURCE "test/*.c*")
source_group("test" FILES ${TEST_SOURCE})

add_executable(tests ${TEST_SOURCE})

add_dependencies(tests googletest glew glfw libpng freetype)

#add folders with gmock headers to "include" section in VS test project
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/json/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/googletest/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/freetype/include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libpng)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng-build/zlib)

target_link_libraries(game opengl32.lib)

#link freetype libraries
target_link_libraries(tests debug ${CMAKE_CURRENT_BINARY_DIR}/freetype-prefix/src/freetype-build/src/Debug/freetyped.lib)
target_link_libraries(tests optimized ${CMAKE_CURRENT_BINARY_DIR}/freetype-prefix/src/freetype-build/src/Release/freetype.lib)

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