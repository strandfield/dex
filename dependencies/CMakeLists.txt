
##################################################################
###### yaml-cpp
##################################################################

find_package(YAMLCPP)

if(NOT YAMLCPP_FOUND)
  include("yaml.cmake")
endif()

##################################################################
###### json-toolkit
##################################################################

if(NOT TARGET json-toolkit)

  file(GLOB_RECURSE JSON_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/json-toolkit/include/*.h)
  add_custom_target(json-toolkit SOURCES ${JSON_HDR_FILES})

  set(JSONTOOLKIT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/json-toolkit/include" PARENT_SCOPE)

#add_library(json-toolkit INTERFACE)
#target_sources(json-toolkit INTERFACE ${JSON_HDR_FILES})

endif()

##################################################################
###### typeset
##################################################################

set(TYPESET_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/typeset" CACHE PATH "Provide the directory of the typeset library")

## Create target

file(GLOB_RECURSE TYPESET_SRC_FILES ${TYPESET_PROJECT_DIR}/src/*.cpp)
file(GLOB_RECURSE TYPESET_HDR_FILES ${TYPESET_PROJECT_DIR}/include/*.h)
add_library(typeset SHARED ${TYPESET_HDR_FILES} ${TYPESET_SRC_FILES})
target_compile_definitions(typeset PRIVATE -DLIBTYPESET_BUILD_LIB)
target_include_directories(typeset PUBLIC "${TYPESET_PROJECT_DIR}/include")

set_target_properties(typeset PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(typeset PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

##################################################################
###### liquid
##################################################################

set(LIQUID_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/liquid" CACHE PATH "Provide the directory of the liquid library")

set(LIQUID_BUILD_TESTS OFF CACHE BOOL "whether to build cxxast test")

add_subdirectory(liquid)

set_target_properties(liquid PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(liquid PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

##################################################################
###### cpptok
##################################################################

set(BUILD_CPPTOK_TESTS OFF CACHE BOOL "whether to build cpptok test")

add_subdirectory(cpptok)

set_target_properties(cpptok PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(cpptok PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
