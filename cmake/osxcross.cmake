# Add stuff here.

#set(wx_root "${OSXCROSS_TARGET_DIR}/macports/pkgs/opt/local/Library/Frameworks/wxWidgets.framework/Versions/wxWidgets/3.1") 
set(wx_root "${OSXCROSS_TARGET_DIR}")
set(wxWidgets_LIB_DIR ${wx_root}/wxwidgets/lib)
set(macports_lib_dir ${wx_root}/macports/pkgs/opt/local/lib)

message("Including OSXCROSS specific wxWidgets")

# wxWidgets auto-discovery doesn't work at all here, so we'll manually reproduce it.  I don't love it.  We may only need this for dynamic libs

#if(LINKING_TYPE MATCHES "static")
#else()
#endif()
set(wxWidgets_FOUND TRUE)
#set(wxWidgets_DEFINITIONS WXUSINGDLL __WXOSX_COCOA__)
#set(wxWidgets_DEFINITIONS_DEBUG)

set(OSX_FRAMEWORKS
	"-framework IOKit"
	"-framework Carbon"
	"-framework Cocoa"
	"-framework AudioToolbox"
	"-framework System"
	"-framework OpenGL"
	)

if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
	message(FATAL_ERROR "Must define CMAKE_OSX_DEPLOYMENT_TARGET")
endif()

set(wxWidgets_LINK_FLAGS
	-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}
	${OSX_FRAMEWORKS}
	)

set(wxWidgets_USE_FILE /usr/share/cmake-3.16/Modules/UsewxWidgets.cmake)

#set(wxWidgets_INCLUDE_DIRS ${wx_root}/lib/wx/include/osx_cocoa-unicode-3.1 ${wx_root}/include/wx-3.1)
set(wxWidgets_INCLUDE_DIRS
	${wx_root}/wxwidgets/lib/wx/include/osx_cocoa-unicode-static-3.1
	${wx_root}/wxwidgets/include/wx-3.1
	)
set(wxWidgets_LIBRARY_DIRS ${wx_root}/lib)
set(wxWidgets_LIBRARIES
	${wxWidgets_LIB_DIR}/libwx_baseu-3.1.a
	${wxWidgets_LIB_DIR}/libwx_baseu_net-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_core-3.1.a
	${wxWidgets_LIB_DIR}/libwx_osx_cocoau_aui-3.1.a
	#${wxWidgets_LIB_DIR}/libxar.a
	${wxWidgets_LIB_DIR}/libwxjpeg-3.1.a
	${wxWidgets_LIB_DIR}/libwxpng-3.1.a
	${wxWidgets_LIB_DIR}/libwxtiff-3.1.a

	${macports_lib_dir}/libiconv.a
	${macports_lib_dir}/libpsl.a
	${macports_lib_dir}/libunistring.a
	#${wxWidgets_LIB_DIR}/lib/jpeg.a
	#${wxWidgets_LIB_DIR}/lib/libpng16.a
	#${wxWidgets_LIB_DIR}/lib/lzma.a
	#${wxWidgets_LIB_DIR}/lib/tiff.a
	#${wxWidgets_LIB_DIR}/lib/zlib.a

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

