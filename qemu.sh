#!/bin/sh
set -e
. ./iso.sh

#cpulimit -l 10 -- qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso -monitor stdio -serial file:serial.log -d int
qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso -monitor stdio -serial file:serial.log -singlestep -d int 2>&1 | tee qemu.log
