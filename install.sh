#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

rm -f jlib.zip
cd jlib
zip -r ../jlib.zip * -x qt/Debug/\* qt/Release/\* 3rdparty/\* obsolete/\* win32/\*
cd ..
rm -f /tmp/jlib.zip
cp jlib.zip /tmp
rm -f jlib.zip
cd /tmp
sudo unzip -o jlib.zip -d /usr/local/include/jlib
rm -f jlib.zip
