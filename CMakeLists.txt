cmake_minimum_required (VERSION 2.8)

project (icd)

include (CheckIncludeFiles)
include (CheckLibraryExists)

# Check for sqlite3 library

check_library_exists(sqlite3 sqlite3_open '' HAVE_SQLITE3_LIB)
if (NOT (${HAVE_SQLITE3_LIB}))
  message(FATAL_ERROR "Missing sqlite3 library")
endif ()

check_include_files(sqlite3.h HAVE_SQLITE3_H)
if (NOT (${HAVE_SQLITE3_H}))
  message(FATAL_ERROR "Missing sqlite3.h header file")
endif ()

# Check for curl library

check_library_exists(curl curl_easy_init '' HAVE_CURL_LIB)
if (NOT (${HAVE_CURL_LIB}))
  message(FATAL_ERROR "Missing curl library")
endif ()

check_include_files(curl/curl.h HAVE_CURL_H)
if (NOT (${HAVE_CURL_H}))
  message(FATAL_ERROR "Missing 'curl/curl.h' header file")
endif ()

# Check for libroxml library

check_library_exists(roxml roxml_load_buf '' HAVE_ROXML_LIB)
if (NOT (${HAVE_ROXML_LIB}))
  message(FATAL_ERROR "Missing roxml library")
endif ()

check_include_files(roxml.h HAVE_ROXML_H)
if (NOT (${HAVE_ROXML_H}))
  message(FATAL_ERROR "Missing roxml.h header file")
endif ()

# Include targets in src

add_subdirectory (src)

# Add required files to 'install' target

install (DIRECTORY scripts/ DESTINATION bin
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )
install (DIRECTORY share/ DESTINATION share/icd
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
  DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  PATTERN "share/*script" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )
install (DIRECTORY www/ DESTINATION /var/www
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
  DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )

# Include 'package' and 'package_source' targets

#set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "icd")
#set (CPACK_PACKAGE_VENDOR "Tomasz Rozensztrauch")
#set (CPACK_SOURCE_PACKAGE_FILE_NAME "icd-${ICD_VERSION_MAJOR}.${ICD_VERSION_MINOR}")
##set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
##set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
#set (CPACK_PACKAGE_VERSION_MAJOR "${ICD_VERSION_MAJOR}")
#set (CPACK_PACKAGE_VERSION_MINOR "${ICD_VERSION_MINOR}")
#set (CPACK_PACKAGE_VERSION_PATCH "0")
##set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
##  set(CPACK_STRIP_FILES "bin/MyExecutable")
##  set(CPACK_SOURCE_STRIP_FILES "")
##set(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")
#set (CPACK_SET_DESTDIR ON)
#include (CPack)

add_custom_target(package
  COMMAND "${PROJECT_SOURCE_DIR}/make-package.sh"
    -b "${PROJECT_BINARY_DIR}"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Building binary package")
