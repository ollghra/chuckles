FROM debian:stable-slim

ARG HOST_UID
ARG HOST_GID
ARG WHO
ARG binutils_version
ARG gcc_version


RUN apt update
RUN apt-get -y install build-essential wget bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
RUN apt-get -y install pigz

# RUN groupadd ${WHO} -g ${HOST_GID}
# RUN useradd -d /home/${WHO} -ms /bin/bash -g ${WHO} ${WHO} 
RUN useradd -d /home/${WHO} ${WHO}
RUN usermod -u ${HOST_UID} ${WHO}
RUN usermod -g ${HOST_GID} ${WHO}

RUN mkdir -p /cross/src/build-binutils
RUN mkdir -p /cross/src/build-gcc

RUN chown -R ${WHO}:${WHO} /cross

USER ${WHO}

WORKDIR /cross/src
RUN wget -c -O gcc-${gcc_version}.tar.gz ftp://ftp.gnu.org/gnu/gcc/gcc-${gcc_version}/gcc-${gcc_version}.tar.gz && tar -I pigz -xf gcc-${gcc_version}.tar.gz
RUN wget -c -O binutils-${binutils_version}.tar.gz ftp://ftp.gnu.org/gnu/binutils/binutils-${binutils_version}.tar.gz && tar -I pigz -xf binutils-${binutils_version}.tar.gz

COPY env/toolchain.sh /tmp

RUN /tmp/toolchain.sh "i686-none-elf"

ENV PATH="/cross/bin:${PATH}"
