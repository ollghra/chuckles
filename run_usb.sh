#!/bin/sh
set -e

. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -hdb $1 -monitor stdio -d int 
