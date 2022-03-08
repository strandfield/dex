
configure_file("dex-output-resources.h.in" "dex-output-resources.h")

add_executable(TEST_dex_output "main.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../catch.hpp" "test-dex-output.cpp"  "${CMAKE_CURRENT_BINARY_DIR}/dex-output-resources.h")
target_include_directories(TEST_dex_output PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.."  "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(TEST_dex_output dex-output dex-model-examples)

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
