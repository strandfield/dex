
find_package(Qt5Core)

##################################################################
###### dex-output library
##################################################################

set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE DEXOUTPUT_LIBRARY_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE DEXOUTPUT_LIBRARY_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

list(FILTER DEXOUTPUT_LIBRARY_SRC_FILES EXCLUDE REGEX "main.cpp")

add_library(dex-output SHARED ${DEXOUTPUT_LIBRARY_HDR_FILES} ${DEXOUTPUT_LIBRARY_SRC_FILES})
target_compile_definitions(dex-output PRIVATE -DDEX_OUTPUT_BUILD_LIB)
target_include_directories(dex-output PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_include_directories(dex-output PUBLIC "${LIQUID_PROJECT_DIR}/include")
target_include_directories(dex-output PUBLIC "${JSONTOOLKIT_INCLUDE_DIRS}")
target_link_libraries(dex-output Qt5::Core)
target_link_libraries(dex-output dex-model)
target_link_libraries(dex-output dex-common)
target_link_libraries(dex-output liquid)

set_target_properties(dex-output PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(dex-output PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
