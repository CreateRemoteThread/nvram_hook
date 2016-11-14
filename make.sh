#!/bin/sh

for i in "$@"
do
case $i in
  mipsle)
  /opt/buildroot-gcc342/bin/mipsel-linux-uclibc-gcc -fno-builtin -I../include -Wall -Wno-trigraphs -O0 -g3 -fPIC -shared -o bins/nvram_hook.so.mipsle nvram_hook.c
  ;;
  mipsbe)
  /opt/toolchains/uclibc-crosstools-gcc-4.4.2-1/usr/bin/mips-linux-uclibc-gcc -fPIC -shared -o bins/nvram_hook.so.mipsbe nvram_hook.c -Wl,-rpath, -L/home/yura/DSL-2880AL_1.00_10232013_GPL/bcm963xx/targets/DSL-2880AL/fs.install/lib -L/home/yura/DSL-2880AL_1.00_10232013_GPL/bcm963xx/targets/DSL-2880AL/fs.install/lib/gpl -L/home/yura/DSL-2880AL_1.00_10232013_GPL/bcm963xx/targets/DSL-2880AL/fs.install/lib/public -lcms_msg -lcms_util -lcms_boardctl -lcrypt -lutil
  ;;
esac
done
