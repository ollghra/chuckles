#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
mkdir -p isodir/modules

cp sysroot/boot/chuckles.kernel isodir/boot/chuckles.kernel
cp -R modules isodir/

cat > isodir/boot/grub/grub.cfg << EOF
menuentry "chuckles" {
	multiboot /boot/chuckles.kernel
	module /modules/program
}
EOF
grub-mkrescue -o chuckles.iso isodir
