cmake_minimum_required(VERSION 3.13)
project(dex)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # disables warning about DLL export 
  # this is annoying when using C++ STL
  add_compile_options(/wd4251)  
endif()

##################################################################
###### C++17 clang (Travis CI)
##################################################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message("Building with clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

##################################################################
###### coverage build
##################################################################

if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

##################################################################
###### dependencies
##################################################################

add_subdirectory(dependencies)

##################################################################
###### Qt
##################################################################

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Gui Test)

##################################################################
###### modules
##################################################################

add_subdirectory(modules)

##################################################################
###### apps
##################################################################

add_subdirectory(apps)

##################################################################
###### tests
##################################################################

add_subdirectory(tests)
