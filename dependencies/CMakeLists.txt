
##################################################################
###### dynlib
##################################################################

set(DYNLIB_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dynlib" CACHE PATH "Provide the directory of the dynlib library")

if ("${DYNLIB_PROJECT_DIR}" STREQUAL "")
  message(FATAL_ERROR "The directory of the dynlib project must be specified")  
endif()

file(GLOB_RECURSE DYNLIB_SRC_FILES ${DYNLIB_PROJECT_DIR}/src/*.cpp)
file(GLOB_RECURSE DYNLIB_HDR_FILES 
  ${DYNLIB_PROJECT_DIR}/include/*.h
  ${DYNLIB_PROJECT_DIR}/src/*.h
)
add_library(dynlib STATIC ${DYNLIB_HDR_FILES} ${DYNLIB_SRC_FILES})
target_include_directories(dynlib PUBLIC "${DYNLIB_PROJECT_DIR}/include")
target_compile_definitions(dynlib PUBLIC -DDYNLIB_STATIC_LINKING)

if (NOT WIN32)
  target_link_libraries(dynlib ${CMAKE_DL_LIBS})
  set_property(TARGET dynlib PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

foreach(_source IN ITEMS ${DYNLIB_HDR_FILES} ${DYNLIB_SRC_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

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
