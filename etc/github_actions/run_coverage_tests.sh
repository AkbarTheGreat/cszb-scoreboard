#!/bin/bash

# Run during the GitHub action for CI -- should be run in a Docker container.

sudo supervisord -c /supervisord.conf
cd out/$1
make -j6 all cszb-scoreboard-xml-coverage

