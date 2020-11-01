
##################################################################
###### dex
##################################################################

add_executable(dex "src/main.cpp")
target_link_libraries(dex dex-app)

set_target_properties(dex PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(dex PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (WIN32)
  set_target_properties(dex PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;%PATH%")
endif()
