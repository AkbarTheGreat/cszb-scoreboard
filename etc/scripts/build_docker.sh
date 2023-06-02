#!/bin/bash

# simple script which just builds all of our standard docker containers.  I run
# this sometimes just to pre-cache all of the containers for future executions.

# Actually builds everything
docker build --target=build_all .

# Create some tags, for convenience.
docker build --target=standard_test -t scoreboard .
docker build --target=macos_test    -t osxcross   .
docker build --target=code_clean    -t code_clean .


