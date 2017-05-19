#########################################################################
# File Name: build.sh
# Author: cuilf
# mail: cuilongfeiii@163.com
# Created Time: Thu 20 Apr 2017 07:59:57 PM CST
#########################################################################
#!/bin/bash

export TARGETFS_DIR=/home/cuilf/targetfs
export PATH=/home/cuilf/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/bin:$PATH

arm-linux-gnueabihf-gcc $1 -o $2  \
                                     -pthread \
                                     -I$TARGETFS_DIR/usr/include/gstreamer-1.0 \
                                     -I$TARGETFS_DIR/usr/lib/gstreamer-1.0/include \
                                     -I$TARGETFS_DIR/usr/include/glib-2.0 \
                                     -I$TARGETFS_DIR/usr/lib/glib-2.0/include \
                                     -L$TARGETFS_DIR/usr/lib \
                                     -lgstreamer-1.0 \
                                     -lffi \
                                     -lgmodule-2.0 \
                                     -lgobject-2.0 \
                                     -lglib-2.0


ls -hl $2
