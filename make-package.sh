#!/bin/sh

print_usage()
{
>&2 cat <<EOF

Usage: ${program_name} [-o|--output-dir OUTPUT_DIR] [-O|--build-dir BUILD_DIR]
  [-h|--help] [-v|--version]

Builds a binary package for icd module. When the project build
directory is different than the source directory (out-of-source build)
then the build directory may be provided in -O|--build-dir parameter.
The output directory can be specified by -o|--output-dir parameter.
If the parameter is not provided then the build directory is used.
The script must be executed from source top directory.

  -o|--output-dir output directory
  -O|--build-dir  project build directory
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
  if [ "$2" != "noexit" ]; then
    exit 1;
  fi
}

program_name=`basename "$0"`

version=`git describe --dirty | sed -e 's/^v\(.*\)$/\1/'`

build_dir=`pwd`

options=`getopt -o o:O:hv --long output-dir:,build-dir:,help,version -- "$@"`
test $? -eq 0 || error "Parsing parameters failed"
eval set -- "$options"
while true ; do
  case "$1" in
    -o|--output-dir) output_dir=`eval cd "$2" && pwd`;
       test $? -eq 0 || error "Invalid output directory specified"; shift 2 ;;
    -O|--build-dir) build_dir=`eval cd "$2" && pwd`;
       test $? -eq 0 || error "Invalid build directory specified"; shift 2 ;;
    -h|--help) print_usage; exit 0 ;;
    -v|--version) print_version; exit 0 ;;
    --) shift; break ;;
    *) error "Parsing parameters failed at '$1'" ;;
  esac
done

test "x$1" = "x" || error "Parsing parameters failed at '$1'"

# Let output_dir default to build_dir
if [ "x${output_dir}" = "x" ]; then
  output_dir="${build_dir}"
fi

cd "${output_dir}"
test $? -eq 0 || error "Changing directory to '${output_dir}' failed"

config_file="${output_dir}/icd-${version}-linux.cmake"
> "${config_file}" cat <<EOF
SET(CPACK_GENERATOR "STGZ")
SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
SET(CPACK_INSTALL_CMAKE_PROJECTS "${build_dir};icd;ALL;/")
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

