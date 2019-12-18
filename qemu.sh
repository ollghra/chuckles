#!/bin/sh
set -e
. ./iso.sh

#cpulimit -l 10 -- qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso -monitor stdio -serial file:serial.log -d int
#qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso -monitor stdio -serial file:serial.log -singlestep -d int 2>&1 | tee qemu.log
qemu-system-$(./target-triplet-to-arch.sh $HOST) -machine pc,accel=kvm  -cdrom chuckles.iso -monitor stdio -serial file:serial.log -singlestep -d int,pcall,guest_errors,cpu_reset -D qemu.log -gdb tcp::1234 -S 2>&1&
gdb --symbols 'sysroot/boot/chuckles.kernel' -e 'target remote localhost:1234'
#pkill qemu-system-$(./target-triplet-to-arch.sh $HOST)  
