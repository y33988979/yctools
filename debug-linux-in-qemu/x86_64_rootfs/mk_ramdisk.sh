#!/bin/bash

ARCH=x86_64
sudo rm -rf rootfs
sudo rm -rf tmpfs
sudo rm -rf ramdisk*

sudo mkdir rootfs
sudo mkdir -p rootfs/proc/
sudo mkdir -p rootfs/sys/
sudo mkdir -p rootfs/tmp/
sudo mkdir -p rootfs/root/
sudo mkdir -p rootfs/var/
sudo mkdir -p rootfs/mnt/

sudo cp etc rootfs/ -arf
sudo mkdir -p rootfs/lib64
sudo mkdir -p rootfs/lib
sudo ln -s /lib64 rootfs/lib/x86_64-linux-gnu

sudo cp ../busybox-1.37.0/_install/*  rootfs/ -raf
## add tools
sudo cp /work/rootfs/rootfs/shared/* rootfs/ -rf
sudo ln -s /loop rootfs/bin/stress1
sudo ln -s /loop rootfs/bin/stress2
sudo ln -s /loop rootfs/bin/stress3
sudo cp ../soft/numactl-2.0.16/_install/bin/* -rf  rootfs/bin -raf
sudo cp ../soft/numactl-2.0.16/_install/lib/* -rf  rootfs/lib64 -raf
sudo cp ../soft/procps-v4.0.3/_install/bin/slabtop -rf  rootfs/bin -raf
#sudo cp ../soft/procps-v4.0.3/_install/bin/top -rf  rootfs/bin -raf
sudo cp ../soft/procps-v4.0.3/_install/lib/* -rf  rootfs/lib64 -raf

sudo cp /work/open_source/kernel/linux-4.18.1/modules/*.ko  rootfs/ -raf
sudo cp /work/yctools/linux-utils/fork3  rootfs/ -raf
sudo cp /work/yctools/linux-utils/fork-signal rootfs/ -raf
sudo cp /work/tmp/a.out rootfs/ -raf
sudo cp /work/tmp/a.sh rootfs/ -raf
sudo cp /work/rootfs/rootfs/tools/ rootfs/ -raf

sudo cp -arf /lib/x86_64-linux-gnu/libc.* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/libc-* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/libm.* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/libm-* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/libt* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/libd* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/libmvec* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/linux-vdso* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/ld-2.31* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/ld-linux-x86-64.* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/libresolv* rootfs/lib64/
sudo cp -arf /lib/x86_64-linux-gnu/libpthread* rootfs/lib64/

sudo echo "insmod ./kslub.ko" >> rootfs/etc/init.d/rcS
sudo rm rootfs/lib/*.a
sudo strip rootfs/lib/*

sudo mkdir -p rootfs/dev/
sudo mknod rootfs/dev/tty1 c 4 1
sudo mknod rootfs/dev/tty2 c 4 2
sudo mknod rootfs/dev/tty3 c 4 3
sudo mknod rootfs/dev/tty4 c 4 4
sudo mknod rootfs/dev/console c 5 1
sudo mknod rootfs/dev/null c 1 3

sudo dd if=/dev/zero of=ramdisk bs=1M count=128
sudo mkfs.ext4 -F ramdisk

sudo mkdir -p tmpfs
sudo mount -t ext4 ramdisk ./tmpfs/  -o loop
sudo cp -raf rootfs/*  tmpfs/
sudo umount tmpfs

sudo gzip --best -c ramdisk > ramdisk-$ARCH.gz

