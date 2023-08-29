#!/bin/bash

# Run during the GitHub action for CI -- should be run in a Docker container.

sudo supervisord -c /supervisord.conf
cd out/$1
ctest -T Test --output-on-failure --no-compress-output --build-config $1

