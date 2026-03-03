#!/bin/bash

export HOST_UID=$(id -u)
export HOST_GID=$(id -g)
export WHO=$(whoami)

export binutils_version="2.34"
export gcc_version="10.1.0"

mkdir -p env/cross
mkdir -p env/cross/src

docker-compose up
