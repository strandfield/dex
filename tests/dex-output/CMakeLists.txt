
add_executable(TEST_dex_output "main.cpp" "test-dex-output.cpp" "test-dex-output.h")
target_link_libraries(TEST_dex_output dex-output)
target_link_libraries(TEST_dex_output Qt5::Test)

if (WIN32)
  set_target_properties(TEST_dex_output PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_output PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

add_test(NAME TEST_dex_output COMMAND TEST_dex_output)