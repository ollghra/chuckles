#!/bin/sh -x
#set -e
 
# Set the versions we will be using.
binutils_version="2.29"
gcc_version="7.2.0"
 
# This script expects the target triplet (e.g. i786-pc-elf) as command line argument.
#target=$1
 
# The tools will be installed in ~/cross/$target.
#prefix=$HOME/opt/cross/
export PREFIX="$HOME/opt/cross"
export TARGET=$1
export PATH="$PREFIX/bin:$PATH"

SRCDIR="$HOME/src"
# First check whether the toolchain was already built on a previous run of this script.
if [ ! -d $PREFIX ]
then
	mkdir -p $SRCDIR
	cd $SRCDIR
 
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
	mkdir -p $SRCDIR/build-binutils
	mkdir -p $SRCDIR/build-gcc
 
	# Build binutils.
	cd $SRCDIR/build-binutils
	sudo rm -rf *
	../binutils-$binutils_version/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror 2>&1
	make  2>&1
	make install 2>&1
	sudo rm -rf *
	
	which -- $TARGET-as || echo $TARGET-as is not in the PATH
	
	# Build gcc and libgcc.
	cd $SRCDIR/build-gcc
	$SRCDIR/gcc-$gcc_version/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers 2>&1
	make all-gcc 2>&1
	make all-target-libgcc 2>&1
	make install-gcc 2>&1
	make install-target-libgcc 2>&1
 
	# Make sure that our cross compiler will be found by creating links.
	# Alternative: Add the $prefix/bin directory to your $PATH.
	sudo ln -s -f $PREFIX/bin/* /usr/local/bin/
fi
 
# Also if the cross compiler has not been freshly build, link it so that it will be found.
sudo ln -s -f $PREFIX/bin/* /usr/local/bin/
