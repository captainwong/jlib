#!/bin/bash

set -e

DST_HOST="192.168.50.107"
IGNORES="qt/Debug qt/Release"

rm -f jlib.zip
cd jlib
zip -r ../jlib.zip * -x $IGNORES
cd ..
scp jlib.zip root@$DST_HOST:/root/projects/jlib
rm -f jlib.zip
ssh root@$DST_HOST "cd /root/projects/jlib && unzip -o jlib.zip -d jlib && rm -f jlib.zip"
