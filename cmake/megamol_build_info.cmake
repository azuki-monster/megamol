# megamol_build_info
# This script will provide several build information as C++ library.
# Usage: link to the megamol_build_info library as normal library and `#include "megamol_build_info.h"`.

# Dependency
include(cmrc/CMakeRC)

# Used directories
set(INFO_SCRIPT_DIR "${CMAKE_SOURCE_DIR}/cmake/megamol_build_info")
set(INFO_RESOURCES_DIR "${CMAKE_BINARY_DIR}/megamol_build_info")

# Define produced files
set(BUILD_INFO_CONFIGURE_TIME_FILES
  ${INFO_RESOURCES_DIR}/MEGAMOL_VERSION_MAJOR
  ${INFO_RESOURCES_DIR}/MEGAMOL_VERSION_MINOR
  ${INFO_RESOURCES_DIR}/MEGAMOL_VERSION_PATCH
  ${INFO_RESOURCES_DIR}/MEGAMOL_VERSION)
set(BUILD_INFO_BUILD_TIME_FILES
  ${INFO_RESOURCES_DIR}/MEGAMOL_BUILD_TIMESTAMP
  ${INFO_RESOURCES_DIR}/MEGAMOL_BUILD_TIME
  ${INFO_RESOURCES_DIR}/MEGAMOL_GIT_HASH
  ${INFO_RESOURCES_DIR}/MEGAMOL_GIT_BRANCH_NAME
  ${INFO_RESOURCES_DIR}/MEGAMOL_GIT_BRANCH_NAME_FULL
  ${INFO_RESOURCES_DIR}/MEGAMOL_GIT_REMOTE_NAME
  ${INFO_RESOURCES_DIR}/MEGAMOL_GIT_REMOTE_URL
  ${INFO_RESOURCES_DIR}/MEGAMOL_GIT_DIFF
  ${INFO_RESOURCES_DIR}/MEGAMOL_GIT_IS_DIRTY
  ${INFO_RESOURCES_DIR}/MEGAMOL_LICENSE
  ${INFO_RESOURCES_DIR}/MEGAMOL_CMAKE_CACHE)

# Set configure time values
file(WRITE ${INFO_RESOURCES_DIR}/MEGAMOL_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
file(WRITE ${INFO_RESOURCES_DIR}/MEGAMOL_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
file(WRITE ${INFO_RESOURCES_DIR}/MEGAMOL_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
file(WRITE ${INFO_RESOURCES_DIR}/MEGAMOL_VERSION "${PROJECT_VERSION}")

option("MM_BUILD_INFO_EXACT_TIME" "Use exact time instead of just hourly timestamps." OFF)

# Run build time script
add_custom_target(megamol_build_info_script
  BYPRODUCTS ${BUILD_INFO_BUILD_TIME_FILES}
  COMMAND ${CMAKE_COMMAND} -D BUILD_INFO_EXACT_TIME=${MM_BUILD_INFO_EXACT_TIME} -P ${INFO_SCRIPT_DIR}/megamol_build_info_script.cmake)

# Resources
cmrc_add_resource_library(megamol_build_info_rc WHENCE ${CMAKE_BINARY_DIR}/megamol_build_info
  ${BUILD_INFO_CONFIGURE_TIME_FILES}
  ${BUILD_INFO_BUILD_TIME_FILES})

# Require build info files generation before resources lib build
add_dependencies(megamol_build_info_rc megamol_build_info_script)
