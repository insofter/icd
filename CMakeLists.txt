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

# sed:package {
# This section is removed form exported source package
# Include 'package' and 'package_source' targets
add_custom_target(package
  COMMAND "${PROJECT_SOURCE_DIR}/make-package.sh"
    -b "${PROJECT_BINARY_DIR}"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Building binary package")
add_custom_target(srcpkg
  COMMAND "${PROJECT_SOURCE_DIR}/make-srcpkg.sh"
    -o "${PROJECT_BINARY_DIR}"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Building source package")
# sed:package }
