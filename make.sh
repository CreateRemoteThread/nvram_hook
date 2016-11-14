#!/bin/sh

/opt/buildroot-gcc342/bin/mipsel-linux-uclibc-gcc -fno-builtin -I../include -Wall -Wno-trigraphs -O0 -g3 -fPIC -shared -o nvram_hook.so nvram_hook.c

