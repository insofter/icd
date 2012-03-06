#!/bin/sh

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

${program_name} ${version}
Copyright (c) 2011-2012 Tomasz Rozensztrauch

EOF
}

info() 
{
  echo "${program_name}: $1" >&2 
}

error() 
{
  echo "${program_name}: Error! $1" >&2
  exit 1 
}

program_name=`basename "$0"`

# Get version from git describe command. This produces a version 
# which is based on last tag in the repositioty. If the currently
# checked out version is not tagged then a suffix is added
# indicating the number of commits from the most recent tagged commit
# plus a unique current commit name plus a 'dirty' flag if the working
# tree has some uncomited changes.
version=`git describe --dirty || echo "dirty"`

# The tag is exected to be in v0.0 or v0.0.0 format. If that is the case
# then the 'v' letter is omitted, otherwies the whole tag is taken as
# the version.
version=`echo "${version}" | sed -e 's/^v\(.*\)$/\1/'`

build_dir=`pwd`

options=`getopt -o b:hv --long build-dir:,help,version -- "$@"`
eval set -- "$options"
while true ; do
  case "$1" in
    -b|--build-dir) build_dir=`cd "$2" && pwd`;
       test $? -eq 0 || error "Invalid build directory specified"; shift 2 ;;
    -h|--help) print_usage; exit 0 ;;
    -v|--version) print_version; exit 0 ;;
    --) shift; break ;;
    *) error "Parsing parameters failed at '$1'" ;;
  esac
done

test "x$1" = "x" || error "Parsing parametes failed at '$1'"

cd "${build_dir}"
test $? -eq 0 || error "Changing pwd to \'${build_dir}\' failed"

config_file="${build_dir}/icd-${version}-linux.cmake"
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
SET(CPACK_PACKAGE_FILE_NAME "icd-${version}-linux")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "icd package")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "/usr/share/cmake-2.8/Templates/CPack.GenericDescription.txt")
SET(CPACK_PACKAGE_DEFAULT_LOCATION "/")
EOF

cpack --config "${config_file}"
test $? -eq 0 || error "Running cpack failed"

exit 0
