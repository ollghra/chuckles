#!/bin/bash

export OSTYPE

docker-compose run --remove-orphans --build env
