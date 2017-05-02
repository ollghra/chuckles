#!/bin/sh
set -e
. ./iso.sh

echo "
gdb
(gdb) target remote localhost:1234 
"

#~/src/cpulimit/src/cpulimit -l 5 qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso -m 256M -monitor stdio -d int
qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso -d int -s -S
