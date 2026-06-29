#!/bin/bash

# Use docker to build coverage for the tests and places it in out/cov

docker buildx build --load --target=generate_cov -t coverage .

docker run -v ${PWD}/out/cov:/cszb-scoreboard/out/Linux/Coverage/cszb-scoreboard-coverage --rm -it coverage 
