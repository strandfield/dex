
enable_testing()

add_executable(tests "main.cpp")
add_dependencies(tests dex-model dex-input dex-output)
target_include_directories(tests PUBLIC "../include")
target_link_libraries(tests dex-model dex-input dex-output)
