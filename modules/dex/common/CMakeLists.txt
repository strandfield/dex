
find_package(Qt5Core)

##################################################################
###### dex-common library
##################################################################

file(GLOB_RECURSE DEXCOMMON_LIBRARY_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE DEXCOMMON_LIBRARY_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(dex-common SHARED ${DEXCOMMON_LIBRARY_HDR_FILES} ${DEXCOMMON_LIBRARY_SRC_FILES})
target_compile_definitions(dex-common PRIVATE -DDEX_COMMON_BUILD_LIB)
target_include_directories(dex-common PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_link_libraries(dex-common Qt5::Core)
target_include_directories(dex-common PUBLIC "${LIQUID_PROJECT_DIR}/json-toolkit/include")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message("GCC compiler: linking to stdc++fs")
    #add_compile_options("-lstdc++fs")
    target_link_libraries(dex-common stdc++fs)
endif()

set_target_properties(dex-common PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(dex-common PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
