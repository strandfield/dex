
##################################################################
###### dex
##################################################################

add_executable(dex "src/main.cpp")
target_link_libraries(dex dex-model dex-input dex-output)

#file(COPY "profiles" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(TARGET dex POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/profiles $<TARGET_FILE_DIR:dex>/profiles)
