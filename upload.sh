#!/bin/bash

set -e

if [ $# -ne 1 ]; then
    DST_HOST='192.168.50.192'
else
    DST_HOST=$1
fi

USER=jack
IGNORES="qt/Debug/\* qt/Release/\* 3rdparty/\* obsolete/\* win32/\*"

rm -f jlib.zip
cd jlib
zip -r ../jlib.zip * -x qt/Debug/\* qt/Release/\* 3rdparty/\* obsolete/\* win32/\*
cd ..
ssh ${USER}@$DST_HOST "rm -f /tmp/jlib.zip"
scp jlib.zip ${USER}@$DST_HOST:/tmp
rm -f jlib.zip
ssh ${USER}@$DST_HOST "cd /tmp && sudo unzip -o jlib.zip -d /usr/local/include/jlib && rm -f jlib.zip"
