#!/bin/sh
set -e
. ./iso.sh

#~/src/cpulimit/src/cpulimit -l 10 qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso
qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso
