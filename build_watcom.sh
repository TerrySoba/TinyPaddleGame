#!/bin/bash

docker run --user $(id -u):$(id -g) -v `pwd`/source:/build open_watcom sh -c "cd /build && make -f Makefile.watcom"