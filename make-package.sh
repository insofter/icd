

tmp_file=`mktemp`

> "${tmp_file}" cat <<CONFIG_FILE
SET(CPACK_GENERATOR "STGZ")
SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
SET(CPACK_INSTALL_CMAKE_PROJECTS "/home/tomasz/projects/output;icd;ALL;/")
SET(CPACK_INSTALL_PREFIX "/usr")
SET(CPACK_SET_DESTDIR "ON")
SET(CPACK_RESOURCE_FILE_LICENSE "/usr/share/cmake-2.8/Templates/CPack.GenericLicense.txt")
SET(CPACK_PACKAGE_NAME "icd")
SET(CPACK_PACKAGE_VENDOR "Tomasz Rozensztrauch")
SET(CPACK_PACKAGE_VERSION "1.0-7-de4523a")
SET(CPACK_PACKAGE_FILE_NAME "icd-1.0.0-Linux")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "--icd--")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "/usr/share/cmake-2.8/Templates/CPack.GenericDescription.txt")
SET(CPACK_PACKAGE_DEFAULT_LOCATION "/")
CONFIG_FILE

cpack --config "${tmp_file}"

rm "${tmp_file}"
