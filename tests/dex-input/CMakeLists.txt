
add_executable(TEST_dex_input "main.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../catch.hpp" "test-dex-input.cpp")
target_include_directories(TEST_dex_input PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(TEST_dex_input dex-input)
target_link_libraries(TEST_dex_input Qt5::Test)

if (WIN32)
  set_target_properties(TEST_dex_input PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_input PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_input PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${YAMLCPP_DIR}/bin;${Qt5_DIR}/../../../bin;%PATH%")
endif()

add_test(NAME TEST_dex_input COMMAND TEST_dex_input)

if (WIN32)
  set_tests_properties(TEST_dex_input PROPERTIES ENVIRONMENT "PATH=${YAMLCPP_DIR}/bin\;${Qt5_DIR}/../../../bin;%PATH%")
endif()
