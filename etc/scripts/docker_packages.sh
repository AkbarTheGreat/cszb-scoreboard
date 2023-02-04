#!/usr/bin/bash

 export DEBIAN_FRONTEND=noninteractive

/usr/bin/apt update
/usr/bin/apt -y upgrade

/usr/bin/apt -y install   \
	clang \
	clang-tidy \
	cmake \
	cmake-curses-gui \
	g++ \
	git \
	gcovr \
	libcairo2-dev \
	libexpat1-dev \
	libgtk-3-dev \
	libjpeg-dev \
	libjsoncpp-dev \
	libmspack-dev \
	libnotify-dev \
	libpng-dev \
	libprotobuf-dev \
	libsdl1.2-dev \
	libsdl2-dev \
	libsecret-1-dev \
	libsoup2.4-dev \
	libssl-dev \
	libtiff-dev \
	libwebkit2gtk-4.0-dev \
	libx11-dev \
	llvm-dev \
	ninja-build \
	openjdk-17-jre \
	protobuf-compiler \
	tightvncserver \
	valgrind

echo "yes" | /usr/bin/cpan install \
	File::Copy::Recursive \
	File::Which \
	List::AllUtils

if [[ "$1" == "osxcross" ]]; then
	/usr/bin/apt -y install \
		curl
#else
# TODO -- break out non-osx dependencies
fi
