cmake_minimum_required(VERSION 3.13)
project(dex)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # disables warning about DLL export 
  # this is annoying when using C++ STL
  add_compile_options(/wd4251)  
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

##################################################################
###### install config
##################################################################

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


##################################################################
###### C++17 clang (Travis CI)
##################################################################

#if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#  message("Building with clang")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string )
  string (REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
  message("Clang version: ${CLANG_VERSION_STRING}")
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

##################################################################
###### install
##################################################################

message("Install RPATH is: ${CMAKE_INSTALL_RPATH}")

install(TARGETS liquid typeset cpptok LIBRARY DESTINATION lib)
install(DIRECTORY "${YAMLCPP_DIR}/bin/" TYPE LIB)
install(TARGETS dex-common dex-model dex-input dex-output dex-app LIBRARY DESTINATION lib)
install(TARGETS dex RUNTIME DESTINATION bin)

if (WIN32)
  configure_file("make-release.bat.in" "${CMAKE_BINARY_DIR}/make-release.bat")
endif()
