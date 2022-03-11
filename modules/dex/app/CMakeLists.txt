
##################################################################
###### dex-app library
##################################################################

file(GLOB_RECURSE DEXAPP_LIBRARY_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE DEXAPP_LIBRARY_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(dex-app SHARED ${DEXAPP_LIBRARY_HDR_FILES} ${DEXAPP_LIBRARY_SRC_FILES})
target_compile_definitions(dex-app PRIVATE -DDEX_APP_BUILD_LIB)
target_include_directories(dex-app PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_link_libraries(dex-app dex-input)
target_link_libraries(dex-app dex-output)

set_target_properties(dex-app PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(dex-app PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
