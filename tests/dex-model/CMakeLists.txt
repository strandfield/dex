
add_executable(TEST_dex_model "main.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../catch.hpp" "test-dex-model.cpp")
target_include_directories(TEST_dex_model PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(TEST_dex_model dex-model)

if (WIN32)
  set_target_properties(TEST_dex_model PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_dex_model PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

add_test(NAME TEST_dex_model COMMAND TEST_dex_model)
