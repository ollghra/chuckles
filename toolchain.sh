#!/bin/sh
set -e
 
# Set the versions we will be using.
binutils_version="2.29"
gcc_version="7.2.0"
 
# This script expects the target triplet (e.g. i786-pc-elf) as command line argument.
target=$1
 
# The tools will be installed in ~/cross/$target.
prefix=~/opt/cross/$target
 
# First check whether the toolchain was already built on a previous run of this script.
if [ ! -d $prefix ]
then
	mkdir -p /tmp/toolchain
	cd /tmp/toolchain
 
	# Download gcc sources if they are not yet downloaded.
	if [ ! -f gcc-$gcc_version.tar.gz ]
	then
		wget -c -O gcc-$gcc_version.tar.gz ftp://ftp.gnu.org/gnu/gcc/gcc-$gcc_version/gcc-$gcc_version.tar.gz
		tar -xf gcc-$gcc_version.tar.gz
	fi
 
	# Download binutils sources if they are not yet downloaded.
	if [ ! -f binutils-$binutils_version.tar.gz ]
	then
		wget -c -O binutils-$binutils_version.tar.gz ftp://ftp.gnu.org/gnu/binutils/binutils-$binutils_version.tar.gz
		tar -xf binutils-$binutils_version.tar.gz
	fi
 
	# Create build paths.
	mkdir -p /tmp/toolchain/build-binutils
	mkdir -p /tmp/toolchain/build-gcc
 
	# Build binutils.
	cd /tmp/toolchain/build-binutils
	sudo rm -rf *
	/tmp/toolchain/binutils-$binutils_version/configure --target=$target --prefix=$prefix --disable-nls 2>&1
	make all 2>&1
	make install 2>&1
	sudo rm -rf *
 
	# Build gcc and libgcc.
	cd /tmp/toolchain/build-gcc
	/tmp/toolchain/gcc-$gcc_version/configure --target=$target --prefix=$prefix --disable-nls --enable-languages=c --without-headers 2>&1
	make all-gcc 2>&1
	make install-gcc 2>&1
	make all-target-libgcc 2>&1
	make install-target-libgcc 2>&1
 
	# Make sure that our cross compiler will be found by creating links.
	# Alternative: Add the $prefix/bin directory to your $PATH.
	sudo ln -s -f $prefix/bin/* /usr/local/bin/
fi
 
# Also if the cross compiler has not been freshly build, link it so that it will be found.
sudo ln -s -f $prefix/bin/* /usr/local/bin/
