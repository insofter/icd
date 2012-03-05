#!/bin/sh

print_info()
{
  echo "${program_name}: $1" >&2
}

print_error()
{
  echo "${program_name}: Error! $1" >&2
}

warn_on_error()
{
  if [ $? -ne 0 ]; then
    print_error "$1"
  fi
}

exit_on_error()
{
  if [ $? -ne 0 ]; then
    print_error "$1"
    echo "Program halted" >&2
    exit 1
  fi
}

print_usage()
{
>&2 cat <<EOF

Usage: ${program_name} [-b|--build-dir BUILD_DIR]
  [-h|--help] [-v|--version]

Builds a binary package for icd project.

Note! This tool must be run from the source top directory.

  -b|--build-dir  project build directory; default is current directory
  -h|--help       show this information
  -v|--version    show version information

EOF
}

print_version()
{
>&2 cat <<EOF

${program_name} ${version} ${build_date}
Copyright (c) 2011-2012 Tomasz Rozensztrauch

EOF
}

program_name=`basename "$0"`
version=`git describe --dirty || echo "dirty"`
build_date=`date`
build_dir=`pwd`

options=`getopt -o s:b:hv --long source-dir:,build-dir:,help,version -- "$@"`
eval set -- "$options"
while true ; do
  case "$1" in
    -b|--build-dir) build_dir=$2; shift 2 ;;
    -h|--help) print_usage; exit 0 ;;
    -v|--version) print_version; exit 0 ;;
    --) shift; break ;;
    *) print_error "Parsing parameters failed at '$1'"; exit 1 ;;
  esac
done

cd "${build_dir}"
exit_on_error "Changing pwd to \'${build_dir}\' failed"

config_file=`mktemp`
> "${config_file}" cat <<EOF
SET(CPACK_GENERATOR "STGZ")
SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
SET(CPACK_INSTALL_CMAKE_PROJECTS "`pwd`;icd;ALL;/")
SET(CPACK_INSTALL_PREFIX "/usr")
SET(CPACK_SET_DESTDIR "ON")
SET(CPACK_RESOURCE_FILE_LICENSE "/usr/share/cmake-2.8/Templates/CPack.GenericLicense.txt")
SET(CPACK_PACKAGE_NAME "icd")
SET(CPACK_PACKAGE_VENDOR "Tomasz Rozensztrauch")
SET(CPACK_PACKAGE_VERSION "${version}")
SET(CPACK_PACKAGE_FILE_NAME "icd-${version}-Linux")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "icd package")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "/usr/share/cmake-2.8/Templates/CPack.GenericDescription.txt")
SET(CPACK_PACKAGE_DEFAULT_LOCATION "/")
EOF

cpack --config "${config_file}"
exit_on_error "Running cpack failed"

rm "${config_file}"
