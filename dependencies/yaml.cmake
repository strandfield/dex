
message("Hello Yaml!")

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}")
  message("Creating ${CMAKE_CURRENT_BINARY_DIR}")
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()

set(WORKDIR "${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp")

if(NOT EXISTS "${WORKDIR}")
  message("Cloning yaml-cpp")
  message("Git executable: ${GIT_EXECUTABLE}")
  execute_process(
    COMMAND             ${GIT_EXECUTABLE} clone https://github.com/jbeder/yaml-cpp.git
    WORKING_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}
    RESULT_VARIABLE     git_result
    OUTPUT_VARIABLE     git_output)
    message("git result: ${git_result}")
    message("git output: ${git_output}")
  execute_process(
    COMMAND             ${GIT_EXECUTABLE} checkout yaml-cpp-0.7.0
    WORKING_DIRECTORY   ${WORKDIR}
    RESULT_VARIABLE     git_result
    OUTPUT_VARIABLE     git_output)
endif()

message("Executing commands in ${WORKDIR}")

set(YAMLCPP_BUILT OFF CACHE BOOL "whether yaml-cpp was built")

if(NOT ${YAMLCPP_BUILT})

  message("Building yaml-cpp in Release mode")

  file(MAKE_DIRECTORY ${WORKDIR}/build)

  execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} "-DCMAKE_BUILD_TYPE=Release" "-DYAML_BUILD_SHARED_LIBS=on" ..
    WORKING_DIRECTORY ${WORKDIR}/build
    RESULT_VARIABLE RESULT_VAR)
  
  execute_process(COMMAND ${CMAKE_COMMAND} --build . --target "yaml-cpp" --config "Release"
    WORKING_DIRECTORY ${WORKDIR}/build
    RESULT_VARIABLE RESULT_VAR)
  
  message("Building yaml-cpp in Debug mode")
  
  file(MAKE_DIRECTORY ${WORKDIR}/build_Debug)
  
  execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} "-DCMAKE_BUILD_TYPE=Debug" "-DYAML_BUILD_SHARED_LIBS=on" ..
    WORKING_DIRECTORY ${WORKDIR}/build_Debug
    RESULT_VARIABLE RESULT_VAR)
  
  execute_process(COMMAND ${CMAKE_COMMAND} --build . --target "yaml-cpp" --config "Debug"
    WORKING_DIRECTORY ${WORKDIR}/build_Debug
    RESULT_VARIABLE RESULT_VAR)
    
  file(MAKE_DIRECTORY ${WORKDIR}/bin)
  file(MAKE_DIRECTORY ${WORKDIR}/lib)
  
  set(YAMLCPP_BUILT ON CACHE BOOL "whether yaml-cpp was built" FORCE)
endif()

if (WIN32)
  file(COPY "${WORKDIR}/build/Release/yaml-cpp.dll" "${WORKDIR}/build_Debug/Debug/yaml-cppd.dll" DESTINATION "${WORKDIR}/bin")
  file(COPY "${WORKDIR}/build/Release/yaml-cpp.lib" "${WORKDIR}/build_Debug/Debug/yaml-cppd.lib" DESTINATION "${WORKDIR}/lib")
else()
  file(COPY "${WORKDIR}/build/Release/yaml-cpp.so" "${WORKDIR}/build_Debug/Debug/yaml-cppd.so" DESTINATION "${WORKDIR}/bin")
endif (WIN32)
  
set(YAMLCPP_INCLUDE "${WORKDIR}/include")
set(YAMLCPP_DIR "${WORKDIR}" PARENT_SCOPE)

if(NOT TARGET YAMLCPP::YAMLCPP)
  add_library(YAMLCPP::YAMLCPP SHARED IMPORTED GLOBAL)
endif()

if (WIN32)
  set_target_properties(YAMLCPP::YAMLCPP PROPERTIES
    IMPORTED_IMPLIB_DEBUG "${WORKDIR}/lib/yaml-cppd.lib"
    IMPORTED_IMPLIB_RELEASE "${WORKDIR}/lib/yaml-cpp.lib"
    IMPORTED_LOCATION_DEBUG "${WORKDIR}/bin/yaml-cppd.dll"
    IMPORTED_LOCATION_RELEASE "${WORKDIR}/bin/yaml-cpp.dll"
    INTERFACE_INCLUDE_DIRECTORIES ${YAMLCPP_INCLUDE})
else()
  set_target_properties(YAMLCPP::YAMLCPP PROPERTIES
    IMPORTED_LOCATION_DEBUG "${YAMLCPP_DIR}/bin/yaml-cppd.so"
    IMPORTED_LOCATION_RELEASE "${YAMLCPP_DIR}/bin/yaml-cpp.so"
    INTERFACE_INCLUDE_DIRECTORIES ${YAMLCPP_INCLUDE})
endif (WIN32)

