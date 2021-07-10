#!/bin/bash

docker build . -t gcc16
docker run --user $(id -u):$(id -g) -v `pwd`/source:/src gcc16 bash -c "cd /src && make test"