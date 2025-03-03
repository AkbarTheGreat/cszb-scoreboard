#!/usr/bin/bash

 export DEBIAN_FRONTEND=noninteractive

/usr/bin/apt update
/usr/bin/apt -y upgrade

/usr/bin/apt -y install   \
	clang \
	clang-tidy \
	cmake \
	gcovr \
	git \
	llvm-dev \
	ninja-build \
	valgrind

echo "yes" | /usr/bin/cpan install \
	File::Copy::Recursive \
	File::Find \
	File::Which \
	List::AllUtils

