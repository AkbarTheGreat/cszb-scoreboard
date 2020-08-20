
#TODO: Build this into a script that can be used to quickly setup cmake if a new host is created

setenv OSXCROSS_HOST x86_64-apple-darwin19 ; setenv OSXCROSS_TARGET_DIR /home/akbar/other_src/osxcross/target/ ; setenv OSXCROSS_TARGET darwin19 ; setenv OSXCROSS_SDK darwin19 
cmake -DCMAKE_TOOLCHAIN_FILE=/home/akbar/other_src/osxcross/target/toolchain.cmake ../..


# For setup, install osxcross, run ./build and add osxcross to your path.  Then run macports for:
# setenv MACOSX_DEPLOYMENT_TARGET 10.7
# osxcross-macports install protobuf3-cpp
# osxcross-macports install gtest
# osxcross-macports install jsoncpp
# osxcross-macports install curl expat 
#
#
# I don't think we want wxwidgets, it only does dylib, which is problematic.
# osxcross-macports install wxWidgets-3.2
# 
#
# So rebuild wxwidgets for osx?  This requires getting all of the submodules: git submodule update --init
# Then, cmake in a new subdirectory:  mkdir osxcross; cd osxcross; cmake -DCMAKE_TOOLCHAIN_FILE=/home/akbar/other_src/osxcross/target/toolchain.cmake ..
# You'll need to edit the config (ccmake) to set Release on and shaerd off.  Also change the install root.
#
#
#
#
# Then you have to fix up Curl by adding cmake files for CURL and setting .so extension to .dylib (for mac compatability)
# 
# cp -R /usr/local/lib/cmake/CURL other_src/osxcross/target/macports/pkgs/opt/local/lib/cmake/CURL
# vim CURLTargets-noconfig.cmake
#
# I had to build protobuf from github to match the macports version
#
