#!/bin/sh

src_dir="$1"
export_dir="$2"

cd "${src_dir}"
test $? -eq 0 || exit 1

version=`git describe --dirty | sed -e 's/^v//' -e 's/+/-/g'`
test -n "${version}" || exit 1

cd "${export_dir}"
test $? -eq 0 || exit 1

mv "update-version.sh" "_update-version.sh"
test $? -eq 0 || exit 1
cat "_update-version.sh" | sed -e 's/^version=.*$/version="'"${version}"'"/' \
  > "update-version.sh"
test $? -eq 0 || exit 1
chmod a+x "update-version.sh"
test $? -eq 0 || exit 1
rm "_update-version.sh"
test $? -eq 0 || exit 1

mv "CMakeLists.txt" "_CMakeLists.txt"
test $? -eq 0 || exit 1
cat "_CMakeLists.txt" | sed -e '/sed:package {/,/sed:package }/ d' > "CMakeLists.txt"
test $? -eq 0 || exit 1
rm "_CMakeLists.txt"
test $? -eq 0 || exit 1

exit 0

