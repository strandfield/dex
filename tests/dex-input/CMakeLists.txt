
add_executable(TEST_dex_input "main.cpp" "test-dex-input.cpp" "test-dex-input.h")
target_link_libraries(TEST_dex_input dex-input)
target_link_libraries(TEST_dex_input Qt5::Test)
