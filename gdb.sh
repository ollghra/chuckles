#!/bin/sh
set -e
. ./iso.sh

echo "
gdb --symbols=isodir/boot/chuckles.kernel --eval-command 'target remote localhost:1234'
"

#~/src/cpulimit/src/cpulimit -l 5 qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso -m 256M -monitor stdio -d int
qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso -d int -s -S
qemu-system-$(./target-triplet-to-arch.sh $HOST) -machine pc,accel=kvm  -cdrom chuckles.iso -serial file:serial.log -s -S -d int,pcall,guest_errors,cpu_reset -m 128 -D qemu.log 2>&1
