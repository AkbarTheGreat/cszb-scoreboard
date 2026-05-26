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

# Try to find protobuf in the osxcross environment.  The protobuf cmake files
# misbehave badly enough we actually need most or all of this, as far as I can
# tell.  It _should_ be as simple as setting Protobuf_DIR, but for some reason, it's not.
set(PROTOBUF_ROOT ${OSXCROSS_TARGET_DIR}/macports/pkgs/opt/local/libexec/protobuf5-cpp)
set(Protobuf_DIR "${PROTOBUF_ROOT}/lib/cmake/protobuf")
set(Protobuf_INCLUDE_DIR "${PROTOBUF_ROOT}/include")
set(Protobuf_INCLUDE_DIRS "${PROTOBUF_ROOT}/include")
set(Protobuf_LIBRARY "${PROTOBUF_ROOT}/lib")
set(Protobuf_LIBRARIES "${PROTOBUF_ROOT}/lib/libprotobuf.dylib")
set(CMAKE_PREFIX_PATH "${PROTOBUF_ROOT}" ${CMAKE_PREFIX_PATH})

set(jsoncpp_DIR ${OSXCROSS_TARGET_DIR}/jsoncpp/lib/cmake/jsoncpp)

if(EXISTS "${Protobuf_DIR}/protobuf-config.cmake")
    message(STATUS "Found Protobuf config: ${Protobuf_DIR}/protobuf-config.cmake")
else()
    message(STATUS "Protobuf config not found at ${Protobuf_DIR}; falling back to module lookup if needed.")
endif()

# I don't know exactly how to make this work better, but it's kind of fragile.
# TODO: Find this cmake file dynamically.
set(wxWidgets_USE_FILE /usr/share/cmake/Modules/UsewxWidgets.cmake)

set(wx_root "${OSXCROSS_TARGET_DIR}/wxwidgets")
set(wxWidgets_LIB_DIR ${wx_root}/lib)
set(macports_lib_dir ${OSXCROSS_TARGET_DIR}/macports/pkgs/opt/local/lib)

# wxWidgets auto-discovery doesn't work at all here, so we'll manually
# reproduce it.  I don't love it, but it is what it is.

set(wxWidgets_FOUND TRUE)
set(wxWidgets_DEFINITIONS __WXOSX_COCOA__)

if(NOT WXWIDGETS_VERSION)
	set(WXWIDGETS_VERSION "3.3")
endif()

set(OSX_FRAMEWORKS
	"-framework AudioToolbox"
	"-framework Carbon"
	"-framework Cocoa"
	"-framework IOKit"
	"-framework OpenGL"
	"-framework System"
	"-framework SystemConfiguration"
	"-framework WebKit"
	)

set(wxWidgets_LINK_FLAGS
	-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}
	${OSX_FRAMEWORKS}
	)


set(wxWidgets_INCLUDE_DIRS
	${wx_root}/lib/wx/include/osx_cocoa-unicode-static-${WXWIDGETS_VERSION}
	${wx_root}/include/wx-${WXWIDGETS_VERSION}
	)

set(wxWidgets_LIBRARY_DIRS ${wx_root}/lib)

# There are likely a bunch of libraries in here we don't need.  The plan is to pare down this list.
set(wxWidgets_LIBRARIES
	${wxWidgets_LIB_DIR}/libwx_baseu-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_baseu_net-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_baseu_xml-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_adv-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_aui-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_core-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_html-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_media-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_propgrid-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_qa-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_ribbon-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_richtext-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_stc-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_webview-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_xrc-${WXWIDGETS_VERSION}-Darwin.a
	${wxWidgets_LIB_DIR}/libwxjpeg-${WXWIDGETS_VERSION}.a
	${wxWidgets_LIB_DIR}/libwxregexu-${WXWIDGETS_VERSION}.a
	${wxWidgets_LIB_DIR}/libwxscintilla-${WXWIDGETS_VERSION}.a
	${wxWidgets_LIB_DIR}/libwxpng-${WXWIDGETS_VERSION}.a
	${wxWidgets_LIB_DIR}/libwxtiff-${WXWIDGETS_VERSION}.a

	# TODO: Alphabetize these libs and deduplicate the list
	${macports_lib_dir}/libiconv.a
	${macports_lib_dir}/libpsl.a
	${macports_lib_dir}/libunistring.a
	${macports_lib_dir}/libbrotlicommon.dylib
	${macports_lib_dir}/libbrotlidec.dylib
	${macports_lib_dir}/libcurl.dylib
	#${macports_lib_dir}/libasprintf.a
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

add_definitions(-D_UNICODE -DUNICODE -DwxUSE_GUI=1 -DWXUSINGDLL -D__WXOSX_COCOA__)
include_directories(SYSTEM INTERFACE ${wxWidgets_INCLUDE_DIRS} ${Protobuf_INCLUDE_DIRS} ${OSX_INCLUDE_DIRS})
link_libraries(${wxWidgets_LINK_FLAGS})

