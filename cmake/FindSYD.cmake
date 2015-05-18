# - Find an SYD installation or build tree.

# When SYD is found, the SYDConfig.cmake file is sourced to setup the
# location and configuration of SYD.  Please read this file, or
# SYDConfig.cmake.in from the SYD source tree for the full list of
# definitions.  Of particular interest is SYD_USE_FILE, a CMake source file
# that can be included to set the include directories, library directories,
# and preprocessor macros.  In addition to the variables read from
# SYDConfig.cmake, this find module also defines
#  SYD_DIR  - The directory containing SYDConfig.cmake.
#             This is either the root of the build tree,
#             or the lib/InsightToolkit directory.
#             This is the only cache entry.
#
#  SYD_FOUND - Whether SYD was found.  If this is true,
#              SYD_DIR is okay.
#

set(SYD_DIR_STRING "directory containing SYDConfig.cmake.  This is either the root of the build tree, or PREFIX/lib for an installation.")

# Search only if the location is not already known.
if(NOT SYD_DIR)
  # Get the system search path as a list.
  if(UNIX)
    string(REGEX MATCHALL "[^:]+" SYD_DIR_SEARCH1 "$ENV{PATH}")
  else(UNIX)
    string(REGEX REPLACE "\\\\" "/" SYD_DIR_SEARCH1 "$ENV{PATH}")
  endif(UNIX)
  string(REGEX REPLACE "/;" ";" SYD_DIR_SEARCH2 ${SYD_DIR_SEARCH1})

  # Construct a set of paths relative to the system search path.
  set(SYD_DIR_SEARCH "")
  foreach(dir ${SYD_DIR_SEARCH2})
    set(SYD_DIR_SEARCH ${SYD_DIR_SEARCH} "${dir}/../lib")
  endforeach(dir)

  #
  # Look for an installation or build tree.
  #
  find_path(SYD_DIR SYDConfig.cmake
    # Look for an environment variable SYD_DIR.
    $ENV{SYD_DIR}

    # Look in places relative to the system executable search path.
    ${SYD_DIR_SEARCH}

    # Look in standard UNIX install locations.
    /usr/local/lib
    /usr/lib

    # Read from the CMakeSetup registry entries.  It is likely that
    # SYD will have been recently built.
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild1]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild2]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild3]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild4]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild5]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild6]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild7]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild8]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild9]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild10]

    # Help the user find it if we cannot.
    DOC "The ${SYD_DIR_STRING}"
  )
endif(NOT SYD_DIR)

# If SYD was found, load the configuration file to get the rest of the
# settings.
if(SYD_DIR)
  set(SYD_FOUND 1)
  include(${SYD_DIR}/SYDConfig.cmake)
else(SYD_DIR)
  set(SYD_FOUND 0)
  if(SYD_FIND_REQUIRED)
    message(FATAL_ERROR "Please set SYD_DIR to the ${SYD_DIR_STRING}")
  endif(SYD_FIND_REQUIRED)
endif(SYD_DIR)

#include(ODB.cmake)
