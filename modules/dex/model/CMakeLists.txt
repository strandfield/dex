
find_package(Qt5Core)

##################################################################
###### dex-model library
##################################################################

file(GLOB_RECURSE DEXOUTPUT_LIBRARY_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE DEXOUTPUT_LIBRARY_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

list(FILTER DEXOUTPUT_LIBRARY_SRC_FILES EXCLUDE REGEX "main.cpp")

add_library(dex-model SHARED ${DEXOUTPUT_LIBRARY_HDR_FILES} ${DEXOUTPUT_LIBRARY_SRC_FILES})
target_compile_definitions(dex-model PRIVATE -DDEX_MODEL_BUILD_LIB)
target_include_directories(dex-model PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../../modules")
target_include_directories(dex-model PUBLIC "${DOM_PROJECT_DIR}/include")
target_include_directories(dex-model PUBLIC "${LIBSCRIPT_PROJECT_DIR}/include")
target_include_directories(dex-model PUBLIC "${LIQUID_PROJECT_DIR}/include")
target_include_directories(dex-model PUBLIC "${LIQUID_PROJECT_DIR}/json-toolkit/include")
target_link_libraries(dex-model typeset)

set_target_properties(dex-model PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(dex-model PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
