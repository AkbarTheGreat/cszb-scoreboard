#!/bin/bash

# simple script which just builds all of our standard docker containers.  I run
# this sometimes just to pre-cache all of the containers for future executions.

# Actually builds everything
docker buildx build --platform=linux/amd64,linux/arm64 --target=build_all .

# Create some tags, for convenience. 
docker buildx build --target=standard_test  --platform=linux/amd64,linux/arm64 -t scoreboard . && \
docker buildx build --target=macos_test     --platform=linux/amd64,linux/arm64 -t osxcross   . && \
docker buildx build --target=code_clean     --platform=linux/amd64,linux/arm64 -t code_clean .


