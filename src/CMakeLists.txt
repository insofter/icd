cmake_minimum_required (VERSION 2.8)

#add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.cc 
#  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/version.cc.in
#    ${CMAKE_CURRENT_BINARY_DIR}/version.cc
#  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/version.cc.in)

#configure_file(
#  ${CMAKE_CURRENT_SOURCE_DIR}/version.cc.in
#  ${CMAKE_CURRENT_BINARY_DIR}/version.cc)
message (STATUS "Updating version")
execute_process(COMMAND "${PROJECT_SOURCE_DIR}/update-version.sh"
    -o "${CMAKE_CURRENT_BINARY_DIR}/version.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/version.cc.in"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_custom_target(update-version
  COMMAND "${PROJECT_SOURCE_DIR}/update-version.sh"
    -o "${CMAKE_CURRENT_BINARY_DIR}/version.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/version.cc.in"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/version.cc.in
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Updating version")

add_library(version ${CMAKE_CURRENT_BINARY_DIR}/version.cc)
add_dependencies(version update-version)

add_library(db-config db-config.cc)
add_library(daemonizer daemonizer.cc)
add_library(syslogstream syslogstream.cc)
add_library(threads threads.cc)
add_library(virtual-device virtual-device.cc)
add_library(itd-vds itd-vds.cc)
#add_library(transfer-agent transfer-agent.cc)
#nowe lcd
add_library(lcd-driver lcd-driver.cpp)
add_library(lcd-menu lcd-menu.cpp)
add_library(lcd-menu-front-item lcd-menu-front-item.cpp)
target_link_libraries(lcd-menu db-config)

add_executable(icd-config icd-config.cc)
target_link_libraries(icd-config db-config sqlite3 version)

add_executable(icd-sql icd-sql.cc)
target_link_libraries(icd-sql sqlite3 version)

add_executable(icd-itd-daemon icd-itd-daemon.cc)
target_link_libraries(icd-itd-daemon itd-vds virtual-device threads
  db-config daemonizer syslogstream sqlite3 version pthread)

add_executable(icd-test icd-test.cc)
target_link_libraries(icd-test db-config sqlite3 version)



#gsoap
add_library(gsoap_C gsoap/C.cpp)
add_library(gsoap_icdtcp3SoapProxy gsoap/icdtcp3SoapProxy.cpp)
add_library(gsoap_stdsoap2 gsoap/stdsoap2.cpp)

add_library(logger logger.cpp)

add_executable(icd-transfer-data icd-transfer-data.cc)
target_link_libraries(icd-transfer-data 
  sqlite3 curl version gsoap_C gsoap_icdtcp3SoapProxy
  gsoap_stdsoap2 logger)
#roxml




#add_executable(icd-fc-daemon icd-fc-daemon.cc)
#target_link_libraries(icd-fc-daemon db-config daemonizer
#  syslogstream sqlite3 version)

#nowe lcd
add_executable(icd-fc-daemon icd-fc-daemon.cc)
target_link_libraries(icd-fc-daemon db-config daemonizer
  syslogstream sqlite3 version lcd-driver lcd-menu lcd-menu-front-item)



install (TARGETS icd-config icd-sql icd-itd-daemon icd-fc-daemon
  icd-transfer-data DESTINATION bin)
