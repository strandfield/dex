
##################################################################
###### dex-output library
##################################################################

file(GLOB DEXOUTPUT_LIBRARY_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB DEXOUTPUT_LIBRARY_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(DEX_EXPORTER_JSON_ENABLED ON)

if(DEX_EXPORTER_JSON_ENABLED)
  file(GLOB_RECURSE DEXOUTPUT_LIBRARY_JSON_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/json/*.cpp)
  file(GLOB_RECURSE DEXOUTPUT_LIBRARY_JSON_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/json/*.h)

  list(APPEND DEXOUTPUT_LIBRARY_SRC_FILES ${DEXOUTPUT_LIBRARY_JSON_SRC_FILES})
  list(APPEND DEXOUTPUT_LIBRARY_HDR_FILES ${DEXOUTPUT_LIBRARY_JSON_HDR_FILES})
endif()

set(DEX_EXPORTER_LIQUID_ENABLED ON CACHE BOOL "enable liquid exporter")

if(DEX_EXPORTER_LIQUID_ENABLED)
  file(GLOB_RECURSE DEXOUTPUT_LIBRARY_LIQUID_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/liquid/*.cpp)
  file(GLOB_RECURSE DEXOUTPUT_LIBRARY_LIQUID_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/liquid/*.h)

  list(APPEND DEXOUTPUT_LIBRARY_SRC_FILES ${DEXOUTPUT_LIBRARY_LIQUID_SRC_FILES})
  list(APPEND DEXOUTPUT_LIBRARY_HDR_FILES ${DEXOUTPUT_LIBRARY_LIQUID_HDR_FILES})
endif()

add_library(dex-output SHARED ${DEXOUTPUT_LIBRARY_HDR_FILES} ${DEXOUTPUT_LIBRARY_SRC_FILES})
target_compile_definitions(dex-output PRIVATE -DDEX_OUTPUT_BUILD_LIB)
target_include_directories(dex-output PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_include_directories(dex-output PUBLIC "${JSONTOOLKIT_INCLUDE_DIRS}")
target_link_libraries(dex-output dex-model)
target_link_libraries(dex-output dex-common)

if(DEX_EXPORTER_LIQUID_ENABLED)
  target_compile_definitions(dex-output PUBLIC DEX_EXPORTER_LIQUID_ENABLED)
  target_include_directories(dex-output PUBLIC "${LIQUID_PROJECT_DIR}/include")
  target_link_libraries(dex-output liquid)
endif()

set_target_properties(dex-output PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(dex-output PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
