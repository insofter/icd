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

Usage: ${program_name} [-h|--help] [-v|--version]

Builds a source package for icd project.

Note! This tool must be run from the source top directory.

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
    -h|--help) print_usage; exit 0 ;;
    -v|--version) print_version; exit 0 ;;
    --) shift; break ;;
    *) print_error "Parsing parameters failed at '$1'"; exit 1 ;;
  esac
done

export_dir=`mktemp -d`

echo "${export_dir}"
rsync -a --exclude=.git* --exclude=/*sh --exclude=/CMakeLists.txt ./ "${export_dir}"

#rm -R "${export_dir}"
