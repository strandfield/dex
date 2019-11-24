cmake_minimum_required(VERSION 3.9)
project(dex)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # disables warning about DLL export 
  # this is annoying when using C++ STL
  add_compile_options(/wd4251)  
endif()

set(CMAKE_AUTOMOC ON)

##################################################################
###### dependencies
##################################################################

add_subdirectory(dependencies)

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
