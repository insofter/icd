#!/bin/sh

print_usage()
{
>&2 cat <<EOF

Usage: ${program_name} [-o|--output OUT_FILE] [IN_FILE]
  [-h|--help] [-v|--version]

Process the input file (or stdin if not provided) and replaces all
ocurrences of @VERSION@ and @DATE@ markers with the current version
and date. The version is calculated based on git describe command.
The resulting file is saved in the output file (or stdout if not
provided).

Note! This tool must be run from the source top directory!
  
  -o|--output     Output file
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

build_date=`date`

options=`getopt -o o:hv --long output:,help,version -- "$@"`
eval set -- "$options"
while true ; do
  case "$1" in
    -o|--output) out_file=$2; shift 2 ;;
    -h|--help) print_usage; exit 0 ;;
    -v|--version) print_version; exit 0 ;;
    --) shift; break ;;
    *) error "Parsing parameters failed at '$1'" ;;
  esac
done

in_file="-"
if [ "x$1" != "x" ]; then
  in_file=$1; shift 1
fi

# Check if no more parameters specified
test "x$1" = "x" || error "Parsing parametes failed at '$1'"

info "Updated version: ${version} ${build_date}"

content=`cat "${in_file}" | sed -e 's/@VERSION@/'"${version}"'/' \
   -e 's/@DATE@/'"${build_date}"'/'`
test "x${content}" != "x" || error "Processing of input file failed"

if [ "${out_file:--}" = "-" ]; then
  echo "${content}"
else
  echo "${content}" > "${out_file}"
fi

