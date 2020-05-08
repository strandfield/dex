
add_executable(TEST_dex_common "main.cpp" "test-dex-common.cpp" "test-dex-common.h")
target_link_libraries(TEST_dex_common dex-common)
target_link_libraries(TEST_dex_common Qt5::Test)

if (WIN32)
  set_target_properties(TEST_dex_common PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_common PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

add_test(NAME TEST_dex_common COMMAND TEST_dex_common)