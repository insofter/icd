cmake_minimum_required (VERSION 2.8)

project (icd)

# Version
set (ICD_VERSION_MAJOR 1)
set (ICD_VERSION_MINOR 0)
set (ICD_COPYRIGHT "Copyright (c) 2011-2012 Tomasz Rozensztrauch")

# Configure a header file to pass some of the CMake settings to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
 
# Add the binary tree to the search path for include files
# so that we will find config.h
include_directories("${PROJECT_BINARY_DIR}")

include(CheckIncludeFiles)
include(CheckLibraryExists)

# Check for sqlite3 library

check_library_exists(sqlite3 sqlite3_open '' HAVE_SQLITE3_LIB)
if(NOT (${HAVE_SQLITE3_LIB}))
  message(FATAL_ERROR "Missing sqlite3 library")
endif()

check_include_files(sqlite3.h HAVE_SQLITE3_H)
if (NOT (${HAVE_SQLITE3_H}))
  message(FATAL_ERROR "Missing sqlite3.h header file")
endif()

# Check for curl library

check_library_exists(curl curl_easy_init '' HAVE_CURL_LIB)
if(NOT (${HAVE_CURL_LIB}))
  message(FATAL_ERROR "Missing curl library")
endif()

check_include_files(curl/curl.h HAVE_CURL_H)
if (NOT (${HAVE_CURL_H}))
  message(FATAL_ERROR "Missing 'curl/curl.h' header file")
endif()

# Check for libroxml library

check_library_exists(roxml roxml_load_buf '' HAVE_ROXML_LIB)
if(NOT (${HAVE_ROXML_LIB}))
  message(FATAL_ERROR "Missing roxml library")
endif()

check_include_files(roxml.h HAVE_ROXML_H)
if (NOT (${HAVE_ROXML_H}))
  message(FATAL_ERROR "Missing roxml.h header file")
endif()


add_library(db-config db-config.cc)
add_library(daemonizer daemonizer.cc)
add_library(syslogstream syslogstream.cc)
add_library(transfer-agent transfer-agent.cc)

add_executable(icd-config icd-config.cc)
target_link_libraries(icd-config db-config sqlite3)

add_executable(icd-sql icd-sql.cc)
target_link_libraries(icd-sql sqlite3)

add_executable(icd-itd-daemon icd-itd-daemon.cc)
target_link_libraries(icd-itd-daemon db-config daemonizer syslogstream sqlite3)

add_executable(icd-fc-daemon icd-fc-daemon.cc)
target_link_libraries(icd-fc-daemon db-config daemonizer syslogstream sqlite3)

add_executable(icd-transfer-data icd-transfer-data.cc)
target_link_libraries(icd-transfer-data transfer-agent sqlite3 curl roxml)


install (TARGETS icd-config icd-sql DESTINATION bin)

include (CPack)
