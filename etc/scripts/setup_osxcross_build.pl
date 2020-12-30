#!/usr/bin/perl

=pod

Currently this is mostly a series of comments tracking my developing
understanding of how to get a Linux machine ready to build via osxcross.  I
hope to eventually turn this into a script that sets osxcross up to be ready
to build the scoreboard.

Copyright 2020 Tracy Beck

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=cut

#TODO: Build this into a script that can be used to quickly setup cmake if a new host is created

=pod

For setup, install osxcross, run ./build and add osxcross to your path.  Then run macports for:
setenv MACOSX_DEPLOYMENT_TARGET 10.7
osxcross-macports install gtest curl expat 
osxcross-macports install zlib gtest libiconv ncurses gettext libunistring libidn2 libffi bzip2 libedit pcre glib2 libpsl openssl curl-ca-bundle curl expat jsoncpp-devel

These two appear to only have dylibs.  So we may need to build them ourselves.

osxcross-macports install protobuf3-cpp
osxcross-macports install jsoncpp

We definitely can't use wxwidgets from macports, due to dylibs.  So build that ourselves

Clone wxwidgets and then get all of the submodules: git submodule update --init
Then, cmake in a new subdirectory:  mkdir osxcross; cd osxcross; cmake -DCMAKE_TOOLCHAIN_FILE=/home/akbar/other_src/osxcross/target/toolchain.cmake ..

You'll need to edit the config (ccmake) to set Release on and shared off.  Also change the install root.

I edited include/wx/osx/private/available.h because the builtin checks were passing spuriously and I was getting an undefined symbol.

Rebuilt wxWidgets and now it sort of runs.

Then you have to fix up Curl by adding cmake files for CURL and setting .so extension to .dylib (for mac compatability)

cp -R /usr/local/lib/cmake/CURL other_src/osxcross/target/macports/pkgs/opt/local/lib/cmake/CURL
vim CURLTargets-noconfig.cmake
(change .so file endings to .a)

The following dylibs wind up being required anyway, so we bundle them with our application:  libcrypto.1.1.dylib  libjsoncpp.24.dylib  libprotobuf.23.dylib  libssl.1.1.dylib  libz.1.dylib

That said, macports protobuf installs the latest protobuf, and the apt version of protobuf will be several versions old, so we do need to rebuild protobuf anyway, to make the protoc/lib versions match, see https://github.com/protocolbuffers/protobuf/blob/master/src/README.md
When you clone protobuf, be sure to checkout the tag which matches the version in osxcross (which can be found with 'x86_64-apple-darwin19-pkg-config --modversion protobuf')

To build the scoreboard:
setenv OSXCROSS_HOST x86_64-apple-darwin19 ; setenv OSXCROSS_TARGET_DIR /home/akbar/other_src/osxcross/target/ ; setenv OSXCROSS_TARGET darwin19 ; setenv OSXCROSS_SDK darwin19 
cmake -DCMAKE_TOOLCHAIN_FILE=/home/akbar/other_src/osxcross/target/toolchain.cmake ../..

=cut
