#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/chuckles.kernel isodir/boot/chuckles.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "chuckles" {
	multiboot /boot/chuckles.kernel
}
EOF
grub-mkrescue -o chuckles.iso isodir
