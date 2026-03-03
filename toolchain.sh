#!/bin/sh -x
set -e

# Set the versions we will be using.
binutils_version="2.34"
gcc_version="10.1.0"

# This script expects the target triplet (e.g. i786-pc-elf) as command line argument.
#target=$1

# The tools will be installed in ~/cross/$target.
#prefix=$HOME/opt/cross/
export PREFIX="/cross"
if [ -z "$1" ]; then
	echo "Please specify target triplet, e.g. i686-pc-elf"
	exit 1
else
	export TARGET=$1
fi
export PATH="$PREFIX/bin:$PATH"

SRCDIR="$PREFIX/src"
# First check whether the toolchain was already built on a previous run of this script.
if [ ! -x "$(command -v ${TARGET}-as)" ] && [ ! -x "$(command -v ${TARGET}-gcc)" ]; then
	mkdir -p $SRCDIR
	cd $SRCDIR

	# Build binutils if not built
	if [ ! -x "$(command -v ${TARGET}-as)" ]; then
		cd $SRCDIR/build-binutils
		rm -rf *
		../binutils-$binutils_version/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror 2>&1
		make -j$(nproc) 2>&1
		make -j$(nproc) install 2>&1
		rm -rf *
	else
		echo "Skipping binutils, already built"
	fi
	which -- $TARGET-as || echo $TARGET-as is not in the PATH

	# Build gcc and libgcc.
	if [ ! -x "$(command -v ${TARGET}-gcc)" ]; then
		cd $SRCDIR/build-gcc
		$SRCDIR/gcc-$gcc_version/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers 2>&1
		make -j$(nproc) all-gcc 2>&1
		make -j$(nproc) all-target-libgcc 2>&1
		make -j$(nproc) install-gcc 2>&1
		make -j$(nproc) install-target-libgcc 2>&1
	else
		echo "Skipping gcc, already built"
	fi

else
	echo "Toolchain for ${TARGET} already built"
fi
