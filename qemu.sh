#!/bin/bash
set -e

if [[ ! -e chuckles.iso ]]; then
	. ./iso.sh
else
	. ./config.sh
fi

if [[ "$HOST_OSTYPE" == "darwin"* || "$OSTYPE" == "darwin"* ]]; then
	echo Running on MacOS
	qemu-system-$(./target-triplet-to-arch.sh $HOST) -display cocoa -machine pc -cdrom chuckles.iso -monitor stdio -serial file:serial.log -d int,pcall,guest_errors,cpu_reset -D qemu.log 2>&1 | tee qemu.log
elif [[ "$HOST_OSTYPE" == "linux-gnu"* || "$OSTYPE" == "linux-gnu"* ]]; then
	echo Running on Linux
	qemu-system-$(./target-triplet-to-arch.sh $HOST) -machine pc,accel=kvm -cdrom chuckles.iso -monitor stdio -serial file:serial.log -singlestep -d int,pcall,guest_errors,cpu_reset -D qemu.log 2>&1 | tee qemu.log
#cpulimit -l 10 -- qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso -monitor stdio -serial file:serial.log -d int
#qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom chuckles.iso -monitor stdio -serial file:serial.log -singlestep -d int 2>&1 | tee qemu.log
#qemu-system-$(./target-triplet-to-arch.sh $HOST) -machine pc,accel=kvm  -cdrom chuckles.iso -monitor stdio -serial file:serial.log -singlestep -d int,pcall,guest_errors,cpu_reset -D qemu.log -gdb tcp::1234 -S 2>&1&
#gdb --symbols 'sysroot/boot/chuckles.kernel' -e 'target remote localhost:1234'
#pkill qemu-system-$(./target-triplet-to-arch.sh $HOST)
else
	echo Couldn\'t identify host platform
fi
