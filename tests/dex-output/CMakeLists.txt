
set(CMAKE_AUTORCC ON)

add_executable(TEST_dex_output "main.cpp" "test-dex-output.cpp" "test-dex-output.h" "test-output-resources.qrc")
target_link_libraries(TEST_dex_output dex-output dex-model-examples)
target_link_libraries(TEST_dex_output Qt5::Test)

target_include_directories(TEST_dex_output PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../dex-model-examples")


if (WIN32)
  set_target_properties(TEST_dex_output PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_output PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_output PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${YAMLCPP_DIR}/bin;${Qt5_DIR}/../../../bin;%PATH%")
endif()

add_test(NAME TEST_dex_output COMMAND TEST_dex_output)

if (WIN32)
  set_tests_properties(TEST_dex_output PROPERTIES ENVIRONMENT "PATH=${YAMLCPP_DIR}/bin\;${Qt5_DIR}/../../../bin")
endif()
