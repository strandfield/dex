
add_executable(TEST_dex_input "main.cpp" "test-dex-input.cpp" "test-dex-input.h")
target_link_libraries(TEST_dex_input dex-input)
target_link_libraries(TEST_dex_input Qt5::Test)

if (WIN32)
  set_target_properties(TEST_dex_input PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_input PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_input PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;%PATH%")
endif()

add_test(NAME TEST_dex_input COMMAND TEST_dex_input)