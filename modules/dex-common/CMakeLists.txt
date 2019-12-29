
find_package(Qt5Core)

##################################################################
###### dex-common library
##################################################################

file(GLOB_RECURSE DEXCOMMON_LIBRARY_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE DEXCOMMON_LIBRARY_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/dex/*.h)

add_library(dex-common SHARED ${DEXCOMMON_LIBRARY_HDR_FILES} ${DEXCOMMON_LIBRARY_SRC_FILES})
target_compile_definitions(dex-common PRIVATE -DDEX_COMMON_BUILD_LIB)
target_include_directories(dex-common PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(dex-common Qt5::Core)
target_include_directories(dex-common PUBLIC "${LIQUID_PROJECT_DIR}/json-toolkit/include")

foreach(_source IN ITEMS ${DEXCOMMON_LIBRARY_HDR_FILES} ${DEXCOMMON_LIBRARY_SRC_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

set_target_properties(dex-common PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(dex-common PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
