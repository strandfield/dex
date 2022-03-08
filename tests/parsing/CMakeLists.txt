
configure_file("dex-parsing-resources.h.in" "dex-parsing-resources.h")

add_executable(TEST_parsing "main.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../catch.hpp" "${CMAKE_CURRENT_BINARY_DIR}/dex-parsing-resources.h" "test-parsing.cpp")
target_include_directories(TEST_parsing PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.."  "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(TEST_parsing dex-input dex-output)

if (WIN32)
  set_target_properties(TEST_parsing PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_parsing PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_parsing PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${YAMLCPP_DIR}/bin;${Qt5_DIR}/../../../bin;%PATH%")
endif()

add_test(NAME TEST_parsing COMMAND TEST_parsing)

if (WIN32)
  set_tests_properties(TEST_parsing PROPERTIES ENVIRONMENT "PATH=${YAMLCPP_DIR}/bin\;${Qt5_DIR}/../../../bin")
endif()
