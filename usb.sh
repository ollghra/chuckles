#!/bin/sh

./iso.sh > /dev/null

echo "writing ./chuckles.iso to $1"

sudo dd bs=4M if=chuckles.iso of=$1
