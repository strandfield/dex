
add_executable(TEST_dex_output "main.cpp" "test-dex-output.cpp" "test-dex-output.h")
target_link_libraries(TEST_dex_output dex-output)
target_link_libraries(TEST_dex_output Qt5::Test)
