#!/bin/sh

print_usage()
{
>&2 cat <<EOF

Usage: ${program_name} [-o|--output-dir BUILD_DIR]
  [-h|--help] [-v|--version]

Builds a source package for icd project.

Note! This tool must be run from the source top directory.

  -o|--output-dir  output directory; default is current directory
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
  exit 1;
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

options=`getopt -o o:hv --long output-dir:,help,version -- "$@"`
eval set -- "$options"

while true ; do
  case "$1" in
    -o|--output-dir) build_dir=`cd "$2" && pwd`;
       test $? -eq 0 || error "Invalid build directory specified"; shift 2 ;;
    -h|--help) print_usage; exit 0 ;;
    -v|--version) print_version; exit 0 ;;
    --) shift; break ;;
    *) error "Parsing parameters failed at '$1'" ;;
  esac
done

test "x$1" = "x" || error "Parsing parametes failedi at '$1'"

temp_dir=`mktemp -d`
test $? -eq 0 || error "Creating temporary directory '${temp_dir}' failed"

export_dir="${temp_dir}/icd-${version}"
mkdir -p "${export_dir}"
test $? -eq 0 || error "Creating '${export_dir}' directory failed"

rsync -a --exclude=.git* --exclude=/*sh --exclude=/CMakeLists.txt ./ "${export_dir}"
test $? -eq 0 || error "Copying package files failed"

cat update-version.sh | sed -e 's/^version=.*$/version="'"${version}"'"/' \
  > ${export_dir}/update-version.sh
chmod a+x ${export_dir}/update-version.sh

cat CMakeLists.txt | sed -e '/sed:package {/,/sed:package }/ d' \
  > ${export_dir}/CMakeLists.txt

cd "${temp_dir}"
test $? -eq 0 || error "Changing pwd to '${build_dir}' failed"

tar -cjf "${build_dir}/icd-${version}.tar.bz2" "icd-${version}"
test $? -eq 0 || \
  error "Creating bzipped tar archive '${build_dir}/icd-${version}.tar.bz2' failed"

rm -R "${temp_dir}"

exit 0
