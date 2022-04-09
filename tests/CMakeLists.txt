
enable_testing()

if(NOT DEFINED DEX_DOWNLOADED_CATCH2 OR NOT DEX_DOWNLOADED_CATCH2)

  message("Downloading Catch2")
  file(DOWNLOAD https://github.com/catchorg/Catch2/releases/download/v2.13.8/catch.hpp "${CMAKE_CURRENT_LIST_DIR}/catch.hpp" SHOW_PROGRESS)
  
  set(DEX_DOWNLOADED_CATCH2 TRUE CACHE BOOL "Whether Catch2 was downloaded")

endif()


add_subdirectory(dex-model-examples)
add_subdirectory(dex-common)
add_subdirectory(dex-model)
add_subdirectory(dex-input)
add_subdirectory(dex-output)
add_subdirectory(parsing)
