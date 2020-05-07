sudo qemu-system-x86_64 \
    -smp 2 \
    -m 1024M \
    -kernel ../build/linux-5.4/arch/x86_64/boot/bzImage \
    -nographic \
    -append "root=/dev/ram0 rw rootfstype=ext4 console=ttyS0 " \
    -initrd /work/rootfs/rootfs/ramdisk.gz \
    -net nic,vlan=0 -net tap,vlan=0,ifname=tap0

