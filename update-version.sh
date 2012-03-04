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
    echo "Program halted" >2
    exit 1
  fi
}

print_usage()
{
  echo ""
  echo "Usage: ${program_name} [-o|--output OUT_FILE] [IN_FILE]"
  echo "  [-h|--help] [-v|--version]"
  echo ""
  echo "Process input file (or stdin if not provided) and replaces ocurrences"
  echo "of @VERSION@ and @DATE@ markers with current version and date."
  echo "The version is calculated based on git describe. The resulting file"
  echo "is saved in output file (or stdout if none provided)."
  echo ""
  echo "  -o|--output     Output file"
  echo "  -h|--help       show this information"
  echo "  -v|--version    show version information"
  echo ""
}

print_version()
{
  echo "${program_name} ${version} ${build_date}"
  echo "Copyright (c) 2011-2012 Tomasz Rozensztrauch"
}

program_name=`basename "$0"`
version=`git describe --dirty`
build_date=`date`

options=`getopt -o o:hv --long output:,help,version -- "$@"`
eval set -- "$options"
while true ; do
  case "$1" in
    -o|--output) out_file=$2; shift 2 ;;
    -h|--help) print_usage; exit 0 ;;
    -v|--version) print_version; exit 0 ;;
    --) shift; break ;;
    *) print_error "Parsing parameters failed at '$1'"; exit 1 ;;
  esac
done

in_file=${1:--}

print_info "Updated version: ${version} ${build_date}"

content=`cat "${in_file}" | sed -e 's/@VERSION@/'"${version}"'/' \
   -e 's/@DATE@/'"${build_date}"'/'`

if [ "${out_file:--}" = "-" ]; then
  echo "${content}"
else
  echo "${content}" > "${out_file}"
fi

