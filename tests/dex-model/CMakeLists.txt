
add_executable(TEST_dex_model "main.cpp" "test-dex-model.cpp" "test-dex-model.h")
target_link_libraries(TEST_dex_model dex-model)
target_link_libraries(TEST_dex_model Qt5::Test)

if (WIN32)
  set_target_properties(TEST_dex_model PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_model PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_model PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;%PATH%")
endif()

add_test(NAME TEST_dex_model COMMAND TEST_dex_model)

if (WIN32)
  set_tests_properties(TEST_dex_model PROPERTIES ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;%PATH%")
endif()
