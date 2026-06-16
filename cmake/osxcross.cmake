# cmake/osxcross.cmake : Cmake configuration for cross-compiling the scoreboard
# for MacOS via osxcross.
# 
# Copyright 2020-2026 Tracy Beck
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#    http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
	message(FATAL_ERROR "Must define CMAKE_OSX_DEPLOYMENT_TARGET")
endif()

if(NOT OSXCROSS_TARGET_DIR)
	message(FATAL_ERROR "Must define OSXCROSS_TARGET_DIR")
endif()

# Protobuf gets really wonky with cross-compliation, especially across platforms.
# For cross-compilation, we need:
# - Native protoc binary (to run on build machine)
# - Target protobuf libraries and headers (for linking)

# Ensure Protobuf_PROTOC_EXECUTABLE is found in native (host) location BEFORE find_package
#find_program(Protobuf_PROTOC_EXECUTABLE protoc PATHS /usr/local/bin /usr/bin NO_CMAKE_FIND_ROOT_PATH REQUIRED)
set(PROTOC_EXECUTABLE "/usr/local/bin/protoc")

message(STATUS "Found Protobuf compiler: ${Protobuf_PROTOC_EXECUTABLE}")

set(OSX_INCLUDE_DIRS "${OSXCROSS_TARGET_DIR}/macports/pkgs/opt/local/include")


set(jsoncpp_DIR ${OSXCROSS_TARGET_DIR}/jsoncpp/lib/cmake/jsoncpp)

set(macports_lib_dir ${OSXCROSS_TARGET_DIR}/macports/pkgs/opt/local/lib)

# The following frameworks and libraries can be found by calling `wx-config --libs`
# on a built wxWidgets, but that doesn't exist before the build, so we just copy
# the results and de-duplicate them here.
set(OSX_FRAMEWORKS
	 "-framework AppIntents"
	 "-framework AppKit"
	 "-framework AudioToolbox"
	 "-framework CoreFoundation"
	 "-framework Carbon"
	 "-framework Cocoa"
	 "-framework IOKit"
	 "-framework OpenGL"
	 "-framework System"
	 "-framework SystemConfiguration"
	 "-framework WebKit"
	)

set(wxWidgets_LINK_FLAGS
	${OSX_FRAMEWORKS}
	${CMAKE_CURRENT_BINARY_DIR}/wxWidgets/lib/libwx_baseu-3.3-Darwin.a
	-mmacosx-version-min=14.0
	)


# There are likely a bunch of libraries in here we don't need.  The plan is to pare down this list.
set(wxWidgets_LIBRARIES
	# TODO: Alphabetize these libs and deduplicate the list
	${macports_lib_dir}/libiconv.a
	${macports_lib_dir}/libpsl.a
	${macports_lib_dir}/libunistring.a
	${macports_lib_dir}/libbrotlicommon.dylib
	${macports_lib_dir}/libbrotlidec.dylib
	${macports_lib_dir}/libcurl.dylib
	${macports_lib_dir}/libbz2.a
	${macports_lib_dir}/libcharset.a
	${macports_lib_dir}/libcrypto.a
	${macports_lib_dir}/libzstd.a
	${macports_lib_dir}/libcurses.a
	${macports_lib_dir}/libedit.a
	${macports_lib_dir}/libexpat.a
	${macports_lib_dir}/libffi.a
	${macports_lib_dir}/libform.a
	${macports_lib_dir}/libformw.a
	${macports_lib_dir}/libgettextpo.a
	${macports_lib_dir}/libgio-2.0.a
	${macports_lib_dir}/libglib-2.0.a
	${macports_lib_dir}/libgmock.a
	${macports_lib_dir}/libgmock_main.a
	${macports_lib_dir}/libgmodule-2.0.a
	${macports_lib_dir}/libgobject-2.0.a
	${macports_lib_dir}/libgtest.a
	${macports_lib_dir}/libgtest_main.a
	${macports_lib_dir}/libgthread-2.0.a
	${macports_lib_dir}/libiconv.a
	${macports_lib_dir}/libidn2.a
	${macports_lib_dir}/libintl.a
	${macports_lib_dir}/libmenu.a
	${macports_lib_dir}/libmenuw.a
	${macports_lib_dir}/libncurses++.a
	${macports_lib_dir}/libncurses.a
	${macports_lib_dir}/libncurses++w.a
	${macports_lib_dir}/libncursesw.a
	${macports_lib_dir}/libpanel.a
	${macports_lib_dir}/libpanelw.a
	${macports_lib_dir}/libpcre16.a
	${macports_lib_dir}/libpcre32.a
	${macports_lib_dir}/libpcre.a
	${macports_lib_dir}/libpcrecpp.a
	${macports_lib_dir}/libpcreposix.a
	${macports_lib_dir}/libpsl.a
	${macports_lib_dir}/libssl.a
	${macports_lib_dir}/libunistring.a
	${macports_lib_dir}/libz.a
	)

find_package(absl REQUIRED CONFIG)

include_directories(SYSTEM INTERFACE ${wxWidgets_INCLUDE_DIRS} ${OSX_INCLUDE_DIRS})
link_libraries(${wxWidgets_LINK_FLAGS})

