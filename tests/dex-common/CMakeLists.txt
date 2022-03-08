
add_executable(TEST_dex_common "main.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../catch.hpp" "test-dex-common.cpp")
target_include_directories(TEST_dex_common PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(TEST_dex_common dex-common)

if (WIN32)
  set_target_properties(TEST_dex_common PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_common PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_common PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;%PATH%")
endif()

add_test(NAME TEST_dex_common COMMAND TEST_dex_common)

if (WIN32)
  set_tests_properties(TEST_dex_common PROPERTIES ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;%PATH%")
endif()
