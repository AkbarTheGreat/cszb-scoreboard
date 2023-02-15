#!/bin/bash

# Command from https://hub.docker.com/r/sickcodes/docker-osx but with a volume for the scoreboard added.

docker run -dit \
	--device /dev/kvm \
	-p 50922:10022 \
	-v /tmp/.X11-unix:/tmp/.X11-unix \
	-e "DISPLAY=${DISPLAY:-:0.0}" \
	-e GENERATE_UNIQUE=true \
	sickcodes/docker-osx:auto


