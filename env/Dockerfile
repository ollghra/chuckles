FROM joshwyant/gcc-cross

RUN apt update
RUN apt-get -y install build-essential wget bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
# RUN apt-get -y install pigz

ENV XCC="i686-elf-gcc"
ARG HOST_OSTYPE

RUN apt-get -y install qemu-system-x86 grub-common xorriso
RUN apt-get -y install grub-pc-bin

