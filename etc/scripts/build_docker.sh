#!/bin/bash

# simple script which just builds all of our standard docker containers.  I run
# this sometimes just to pre-cache all of the containers for future executions.

export PLATFORMS=linux/amd64
# If using a detached builder, you can change to PLATFORMS=linux/amd64,linux/arm64

# Exit on first error, for easier error tracking
set -e

# Actually builds everything
docker buildx build --platform=${PLATFORMS} --target=build_all .

# Create some tags, for convenience. 
docker buildx build --target=standard_test  --platform=${PLATFORMS} -t scoreboard . && \
docker buildx build --target=macos_test     --platform=${PLATFORMS} -t osxcross   . && \
docker buildx build --target=code_clean     --platform=${PLATFORMS} -t code_clean .
