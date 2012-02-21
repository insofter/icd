cmake_minimum_required (VERSION 2.8)

# Configure a header file to pass some of the CMake settings to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
 
# Add the binary tree to the search path for include files
# so that we will find config.h
include_directories("${PROJECT_BINARY_DIR}")

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
