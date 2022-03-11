
##################################################################
###### dex-input library
##################################################################

file(GLOB_RECURSE DEXINPUT_LIBRARY_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE DEXINPUT_LIBRARY_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

list(FILTER DEXINPUT_LIBRARY_SRC_FILES EXCLUDE REGEX "main.cpp")

add_library(dex-input SHARED ${DEXINPUT_LIBRARY_HDR_FILES} ${DEXINPUT_LIBRARY_SRC_FILES})
target_compile_definitions(dex-input PRIVATE -DDEX_INPUT_BUILD_LIB)
target_include_directories(dex-input PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../modules")
target_include_directories(dex-input PUBLIC "${LIQUID_PROJECT_DIR}/include")
target_include_directories(dex-input PUBLIC "${JSONTOOLKIT_INCLUDE_DIRS}")
target_include_directories(dex-input PUBLIC "${TYPESET_PROJECT_DIR}/include")
target_link_libraries(dex-input dex-model)
target_link_libraries(dex-input dex-common)
target_link_libraries(dex-input typeset cpptok)

#foreach(_source IN ITEMS ${DEXINPUT_LIBRARY_HDR_FILES} ${DEXINPUT_LIBRARY_SRC_FILES})
#    get_filename_component(_source_path "${_source}" PATH)
#    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
#    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
#    source_group("${_group_path}" FILES "${_source}")
#endforeach()

set_target_properties(dex-input PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(dex-input PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
