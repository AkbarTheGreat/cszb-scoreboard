#!/bin/bash

# Use docker to build coverage for the tests and places it in out/cov

docker buildx build --target=standard_test -t scoreboard .

docker run -v ${PWD}/out/cov:/cszb-scoreboard/out/Linux/Coverage/cszb-scoreboard-coverage --rm -it scoreboard \
    bash -c \
    'cmake --preset Linux-Coverage && \
    cmake --build --preset Linux-Coverage'


