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
provided). The script must be executed from source top directory.
  
  -o|--output     output file
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
if [ "x${version}" = "x" ]; then
  if [ -f "./VERSION" ]; then
    version=`cat VERSION`
  else
    version="?"
  fi
fi

build_date=`date +'%Y/%m/%d %H:%M %Z'`

options=`getopt -o o:hv --long output:,help,version -- "$@"`
test $? -eq 0 || error "Parsing parameters failed"
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
   -e 's|@DATE@|'"${build_date}"'|'`
test "x${content}" != "x" || error "Processing of input file failed"

if [ "${out_file:--}" = "-" ]; then
  echo "${content}"
else
  echo "${content}" > "${out_file}"
fi

