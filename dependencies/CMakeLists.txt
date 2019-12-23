
##################################################################
###### dom
##################################################################

set(DOM_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dom" CACHE PATH "Provide the directory of the dom library")

if ("${DOM_PROJECT_DIR}" STREQUAL "")
  message(FATAL_ERROR "The directory of the dom project must be specified")  
endif()

## Create dom target

file(GLOB_RECURSE DOM_SRC_FILES ${DOM_PROJECT_DIR}/src/*.cpp)
file(GLOB_RECURSE DOM_HDR_FILES 
  ${DOM_PROJECT_DIR}/include/*.h
  ${DOM_PROJECT_DIR}/src/*.h
)

add_library(dom SHARED ${DOM_HDR_FILES} ${DOM_SRC_FILES})
target_include_directories(dom PUBLIC "${DOM_PROJECT_DIR}/include")
target_compile_definitions(dom PRIVATE -DDOM_BUILD_LIB)

foreach(_source IN ITEMS ${DOM_HDR_FILES} ${DOM_SRC_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

set_target_properties(dom PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(dom PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

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

# json-toolkit

file(GLOB_RECURSE JSON_HDR_FILES ${LIQUID_PROJECT_DIR}/json-toolkit/include/*.h)

add_custom_target(json-toolkit SOURCES ${JSON_HDR_FILES})

## Create target

file(GLOB_RECURSE LIQUID_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/liquid/src/*.cpp)
file(GLOB_RECURSE LIQUID_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/liquid/include/*.h)
add_library(liquid SHARED ${LIQUID_HDR_FILES} ${LIQUID_SRC_FILES})
target_compile_definitions(liquid PRIVATE -DLIQUID_BUILD_SHARED_LIBRARY)
target_compile_definitions(liquid PUBLIC -DJSONTOOLKIT_HAS_CONFIG)
target_include_directories(liquid PUBLIC "${LIQUID_PROJECT_DIR}/json-toolkit/include")
target_include_directories(liquid PUBLIC "${LIQUID_PROJECT_DIR}/include")

set_target_properties(liquid PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(liquid PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
