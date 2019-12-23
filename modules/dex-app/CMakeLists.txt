
find_package(Qt5Core)

##################################################################
###### dex-app library
##################################################################

file(GLOB_RECURSE DEXAPP_LIBRARY_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE DEXAPP_LIBRARY_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/dex/*.h)

add_library(dex-app SHARED ${DEXAPP_LIBRARY_HDR_FILES} ${DEXAPP_LIBRARY_SRC_FILES})
target_compile_definitions(dex-app PRIVATE -DDEX_APP_BUILD_LIB)
target_include_directories(dex-app PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(dex-app Qt5::Core)
target_link_libraries(dex-app dex-input)
target_link_libraries(dex-app dex-output)

foreach(_source IN ITEMS ${DEXAPP_LIBRARY_HDR_FILES} ${DEXAPP_LIBRARY_SRC_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

set_target_properties(dex-app PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(dex-app PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
