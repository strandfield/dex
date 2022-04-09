
include(FindPackageHandleStandardArgs)

if (DEFINED ENV{YAMLCPP_DIR})

  message("Searching yaml-cpp library...")
  find_library(YAMLCPP_LIBRARY NAMES yaml-cpp PATHS ENV YAMLCPP_DIR PATH_SUFFIXES "bin" "lib" REQUIRED)
  find_library(YAMLCPP_LIBRARY_DEBUG NAMES yaml-cppd PATHS ENV YAMLCPP_DIR PATH_SUFFIXES "bin" "lib" REQUIRED)
  find_path(YAMLCPP_INCLUDE NAMES "yaml-cpp/yaml.h" PATHS ENV YAMLCPP_DIR PATH_SUFFIXES "include" REQUIRED)
  #find_path(YAMLCPP_BINDIR NAMES "bin/yaml-cpp.dll" PATHS ENV YAMLCPP_DIR REQUIRED)
  get_filename_component(YAMLCPP_DIR ${YAMLCPP_INCLUDE} DIRECTORY)
  
  find_package_handle_standard_args(YAMLCPP REQUIRED_VARS YAMLCPP_LIBRARY YAMLCPP_INCLUDE)
  
  if (YAMLCPP_FOUND)
    mark_as_advanced(YAMLCPP_LIBRARY)
    mark_as_advanced(YAMLCPP_LIBRARY_DEBUG)
    message("YAMLCPP found: ${YAMLCPP_DIR}")
  else()
    message("Could not find YAMLCPP, please specify YAMLCPP_DIR env variable.")
  endif()
  
  if (YAMLCPP_FOUND AND NOT TARGET YAMLCPP::YAMLCPP)
    add_library(YAMLCPP::YAMLCPP SHARED IMPORTED GLOBAL)
    
    set_target_properties(YAMLCPP::YAMLCPP PROPERTIES
      IMPORTED_IMPLIB_DEBUG ${YAMLCPP_LIBRARY_DEBUG}
      IMPORTED_IMPLIB_RELEASE ${YAMLCPP_LIBRARY}
      IMPORTED_LOCATION_DEBUG "${YAMLCPP_DIR}/bin/yaml-cppd.dll"
      IMPORTED_LOCATION_RELEASE "${YAMLCPP_DIR}/bin/yaml-cpp.dll"
      INTERFACE_INCLUDE_DIRECTORIES ${YAMLCPP_INCLUDE})
    
    set(YAMLCPP_DIR "$ENV{YAMLCPP_DIR}" PARENT_SCOPE)
  endif()

endif()
