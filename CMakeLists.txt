cmake_minimum_required(VERSION 3.9)
project(dex)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # disables warning about DLL export 
  # this is annoying when using C++ STL
  add_compile_options(/wd4251)  
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
