#!/bin/bash

if [ -z "$1" ]
then
      path="."
else
      path=$1
fi

# Replace tabs with 4 spaces in the *.c and *.h files
find $path -name "*.c" -o -name "*.h" -and ! -name "*.git" ! -type d -exec bash -c 'expand -t 4 "$0" > /tmp/e && mv /tmp/e "$0"' {} \;

# Remove trailing spaces
find $path -name '*.c' -o -name "*.h" -and ! -name "*.git" ! -type d -print0 | xargs -r0 sed -e 's/[[:blank:]]\+$//' -i
