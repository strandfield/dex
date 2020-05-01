
set(CMAKE_AUTORCC ON)

add_executable(TEST_parsing "main.cpp" "test-parsing.cpp" "test-parsing.h" "test-parsing-resources.qrc")
target_link_libraries(TEST_parsing dex-input dex-output)
target_link_libraries(TEST_parsing Qt5::Core Qt5::Test)

if (WIN32)
  set_target_properties(TEST_parsing PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_parsing PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

add_test(NAME TEST_parsing COMMAND TEST_parsing)
