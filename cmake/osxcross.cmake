# cmake/osxcross.cmake : Cmake configuration for cross-compiling the scoreboard
# for MacOS via osxcross.
# 
# Copyright 2020 Tracy Beck
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

# I don't know exactly how to make this work better, but it's kind of fragile.
# TODO: Find this cmake file dynamically.
set(wxWidgets_USE_FILE /usr/share/cmake-3.16/Modules/UsewxWidgets.cmake)

set(wx_root "${OSXCROSS_TARGET_DIR}/wxwidgets")
set(wxWidgets_LIB_DIR ${wx_root}/lib)
set(macports_lib_dir ${OSXCROSS_TARGET_DIR}/macports/pkgs/opt/local/lib)

message("Including OSXCROSS specific wxWidgets configuration.")

# wxWidgets auto-discovery doesn't work at all here, so we'll manually
# reproduce it.  I don't love it, but it is what it is.

set(wxWidgets_FOUND TRUE)
set(wxWidgets_DEFINITIONS __WXOSX_COCOA__)

set(OSX_FRAMEWORKS
	"-framework IOKit"
	"-framework Carbon"
	"-framework Cocoa"
	"-framework AudioToolbox"
	"-framework System"
	"-framework OpenGL"
	)

set(wxWidgets_LINK_FLAGS
	-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}
	${OSX_FRAMEWORKS}
	)


set(wxWidgets_INCLUDE_DIRS
	${wx_root}/lib/wx/include/osx_cocoa-unicode-static-3.1
	${wx_root}/include/wx-3.1
	)

set(wxWidgets_LIBRARY_DIRS ${wx_root}/lib)

# There are likely a bunch of libraries in here we don't need.  The plan is to pare down this list.
set(wxWidgets_LIBRARIES
	${wxWidgets_LIB_DIR}/libwx_baseu-3.1.a
	${wxWidgets_LIB_DIR}/libwx_baseu_net-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_core-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_aui-3.1.a
	${wxWidgets_LIB_DIR}/libwxjpeg-3.1.a
	${wxWidgets_LIB_DIR}/libwxpng-3.1.a
	${wxWidgets_LIB_DIR}/libwxtiff-3.1.a

	${macports_lib_dir}/libiconv.a
	${macports_lib_dir}/libpsl.a
	${macports_lib_dir}/libunistring.a

	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_core-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_propgrid-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_richtext-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_xrc-3.1.a
	${wxWidgets_LIB_DIR}/libwxscintilla-3.1.a
	${wxWidgets_LIB_DIR}/libwx_baseu_net-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_adv-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_html-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_qa-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_stc-3.1.a
	${wxWidgets_LIB_DIR}/libwx_baseu_xml-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_aui-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_media-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_ribbon-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_webview-3.1.a
	${wxWidgets_LIB_DIR}/libwxregexu-3.1.a

	${macports_lib_dir}/libasprintf.a
	${macports_lib_dir}/libbz2.a
	${macports_lib_dir}/libcharset.a
	${macports_lib_dir}/libcrypto.a
	${macports_lib_dir}/libcurl.a
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
	${macports_lib_dir}/libprotobuf.a
	${macports_lib_dir}/libprotobuf-lite.a
	${macports_lib_dir}/libprotoc.a
	${macports_lib_dir}/libpsl.a
	${macports_lib_dir}/libssl.a
	${macports_lib_dir}/libunistring.a
	${macports_lib_dir}/libz.a
	)

add_definitions(-D_UNICODE -DUNICODE -DwxUSE_GUI=1 -DWXUSINGDLL -D__WXOSX_COCOA__)
include_directories(SYSTEM INTERFACE ${wxWidgets_INCLUDE_DIRS})
link_libraries(${wxWidgets_LINK_FLAGS})

