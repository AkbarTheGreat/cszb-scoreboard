#!/usr/bin/bash

 export DEBIAN_FRONTEND=noninteractive

/usr/bin/apt update
/usr/bin/apt -y upgrade

/usr/bin/apt -y install   \
	clang \
	cmake \
	git \
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
	ninja-build

echo "yes" | /usr/bin/cpan install \
	File::Copy::Recursive \
	File::Which \
	List::AllUtils

if [[ "$1" == "osxcross" ]]; then
	/usr/bin/apt -y install \
		curl \
		zip

	/usr/bin/cpan install \
		JSON
else
	/usr/bin/apt -y install \
		clang-tidy \
		gcovr \
		protobuf-compiler \
		tightvncserver \
		valgrind
fi

# You can run this with the dev argument in any existing container and get a bunch of interactive tools
# that Akbar likes to have handy.  This is useless for headless runs, so I pulled them out.
if [[ "$1" == "dev" ]]; then
	/usr/bin/apt -y install \
		curl \
		cmake-curses-gui \
		screen \
		vim

	/usr/bin/cpan install \
		CPAN \
		Perl::Tidy
fi

