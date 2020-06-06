#!/bin/sh
set -e
. ./iso.sh

valgrind --leak-check=full --track-origins=yes -s -- qemu-system-$(./target-triplet-to-arch.sh $HOST) -machine pc,accel=kvm -cdrom chuckles.iso -serial file:serial.log -singlestep -d int,pcall,guest_errors,cpu_reset -D qemu.log 2>&1
