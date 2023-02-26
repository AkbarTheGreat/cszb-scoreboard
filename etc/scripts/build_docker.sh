#!/bin/bash

# simple script which just builds all of our standard docker containers.  I run
# this sometimes just to pre-cache all of the containers for future executions.

docker build -f Dockerfile.standard -t scoreboard .
docker build -f Dockerfile.osxcross -t osxcross .
docker build -f Dockerfile.code_clean -t code_clean .


