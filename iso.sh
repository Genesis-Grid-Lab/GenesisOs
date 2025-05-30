#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/GenDevOs.kernel isodir/boot/GenDevOs.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "GenDevOs" {
	multiboot /boot/GenDevOs.kernel
}
EOF
grub-mkrescue -o GenDevOs.iso isodir
