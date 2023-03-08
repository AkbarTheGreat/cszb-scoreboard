# Sourced from https://github.com/asjadenet/wxhellovcpkg/blob/master/winmsvc.cmake on 23-11-2019.
# Licensed via the MIT license, Copyright (c) 2018 Tiit Ülejõe

# Modifications made later by Tracy Beck for cszb-scoreboard, under the Apache license.

# Set WXWIDGETS_INSTALL_DIR to wherever WxWidgets is installed on your local machine for non-vcpkg builds.

if(NOT LINKING_TYPE)
    message(FATAL_ERROR "Please specify -DLINKING_TYPE=static or -DLINKING_TYPE=dynamic")
    return()
endif()

if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Please specify CMAKE_TOOLCHAIN_FILE, for example -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
    message(FATAL_ERROR "Please clean CMake cache and start again")
    return()
endif()


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(LINKING_TYPE MATCHES "static")
        set(VCPKG_TARGET_TRIPLET "x64-windows-static")
    else()
        set(VCPKG_TARGET_TRIPLET "x64-windows")
    endif()
else()
    if(LINKING_TYPE MATCHES "static")
        set(VCPKG_TARGET_TRIPLET "x86-windows-static")
    else()
        set(VCPKG_TARGET_TRIPLET "x86-windows")
    endif()
endif()

message(STATUS "VCPKG_TARGET_TRIPLET:${VCPKG_TARGET_TRIPLET}")

if(NOT WXWIDGETS_VERSION)
	set(WXWIDGETS_VERSION "31")
endif()

if (VCPKG_TARGET_TRIPLET MATCHES "static")
    message(STATUS "static")
    add_definitions(-D_UNICODE -DUNICODE -DwxUSE_GUI=1 -D__WXMSW__)
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
      string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
else()
    message(STATUS "dynamic")
    add_definitions(-D_UNICODE -DUNICODE -DWXUSINGDLL -DwxUSE_GUI=1 -D__WXMSW__)
endif()

if (WXWIDGETS_INSTALL_DIR)
	set (wxWidgets_wxrc_EXECUTABLE "${WXWIDGETS_INSTALL_DIR}/bin/wxrc.exe")
endif()

set(VCPKG_BASE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
	add_definitions(-D_DEBUG)
	if (WXWIDGETS_INSTALL_DIR)
		set(wxWidgets_LIB_DIR  "${WXWIDGETS_INSTALL_DIR}/lib/vc_x64_lib")
		include_directories(SYSTEM INTERFACE ${wxWidgets_LIB_DIR}/mswud ${WXWIDGETS_INSTALL_DIR}/include ${VCPKG_BASE}/include)
		set(wxWidgets_LIBRARIES 
			${wxWidgets_LIB_DIR}/wxjpegd.lib
			${wxWidgets_LIB_DIR}/wxpngd.lib
			${wxWidgets_LIB_DIR}/wxtiffd.lib
			${wxWidgets_LIB_DIR}/wxzlibd.lib
		)
		if (IMAGE_SEARCH)
			set(wxWidgets_LIBRARIES
				${wxWidgets_LIBRARIES}
				${wxWidgets_LIB_DIR}/WebView2LoaderStatic.lib
			)
		endif() # if IMAGE_SEARCH
	else() # Using VCPKG
		set(wxWidgets_LIB_DIR ${VCPKG_BASE}/debug/lib)
		include_directories(SYSTEM INTERFACE ${VCPKG_BASE}/debug/lib/mswud ${VCPKG_BASE}/include)
		set(wxWidgets_LIBRARIES 
			${wxWidgets_LIB_DIR}/jpeg.lib
			${wxWidgets_LIB_DIR}/libpng16d.lib
			${wxWidgets_LIB_DIR}/lzmad.lib
			${wxWidgets_LIB_DIR}/tiffd.lib
			${wxWidgets_LIB_DIR}/zlibd.lib
		)
	endif() # if WXWIDGETS_INSTALL_DIR
	if(LINKING_TYPE MATCHES "static")
		set(wxWidgets_LIBRARIES 
			${wxWidgets_LIBRARIES}
			${wxWidgets_LIB_DIR}/wxbase${WXWIDGETS_VERSION}ud_net.lib
			${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VERSION}ud_aui.lib
			${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VERSION}ud_core.lib
			${wxWidgets_LIB_DIR}/wxbase${WXWIDGETS_VERSION}ud.lib
			${wxWidgets_LIB_DIR}/wxregexud.lib
		)
		if (IMAGE_SEARCH)
			set(wxWidgets_LIBRARIES
				${wxWidgets_LIBRARIES}
				${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VERSION}ud_webview.lib
			)
		endif() # if IMAGE_SEARCH
	else() # Dynamic linking
		message(FATAL_ERROR "Dynamic linking not currently supported.")
	endif() # if LINKING_TYPE == "static"
elseif("${CMAKE_BUILD_TYPE}" MATCHES "Release") # Not Debug, check Release
	set(VCPKG_BASE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})
	if (WXWIDGETS_INSTALL_DIR)
		set(wxWidgets_LIB_DIR  "${WXWIDGETS_INSTALL_DIR}/lib/vc_x64_lib")
		include_directories(SYSTEM INTERFACE ${wxWidgets_LIB_DIR}/mswu ${WXWIDGETS_INSTALL_DIR}/include ${VCPKG_BASE}/include)
		set(wxWidgets_LIBRARIES 
			${wxWidgets_LIB_DIR}/wxjpeg.lib
			${wxWidgets_LIB_DIR}/wxpng.lib
			${wxWidgets_LIB_DIR}/wxtiff.lib
			${wxWidgets_LIB_DIR}/wxzlib.lib
		)
		if (IMAGE_SEARCH)
			set(wxWidgets_LIBRARIES
				${wxWidgets_LIBRARIES}
				${wxWidgets_LIB_DIR}/WebView2LoaderStatic.lib
			)
		endif() # if IMAGE_SEARCH
	else() # Using VCPKG
		set(wxWidgets_LIB_DIR ${VCPKG_BASE}/lib)
		include_directories(SYSTEM INTERFACE ${VCPKG_BASE}/lib/mswu ${VCPKG_BASE}/inclde)
		set(wxWidgets_LIBRARIES 
			${wxWidgets_LIB_DIR}/jpeg.lib
			${wxWidgets_LIB_DIR}/libpng16.lib
			${wxWidgets_LIB_DIR}/lzma.lib
			${wxWidgets_LIB_DIR}/tiff.lib
			${wxWidgets_LIB_DIR}/zlib.lib
		)
	endif() # if WXWIDGETS_INSTALL_DIR
	if(LINKING_TYPE MATCHES "static")
		set(wxWidgets_LIBRARIES 
			${wxWidgets_LIBRARIES}
			${wxWidgets_LIB_DIR}/wxbase${WXWIDGETS_VERSION}u_net.lib
			${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VERSION}u_aui.lib
			${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VERSION}u_core.lib
			${wxWidgets_LIB_DIR}/wxbase${WXWIDGETS_VERSION}u.lib
			${wxWidgets_LIB_DIR}/wxregexu.lib
		)
		if (IMAGE_SEARCH)
			set(wxWidgets_LIBRARIES
				${wxWidgets_LIBRARIES}
				${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VERSION}u_webview.lib
			)
		endif() # if IMAGE_SEARCH
	else() # Dynamic linking
		message(FATAL_ERROR "Dynamic linking not currently supported.")
	endif() # if LINKING_TYPE == "static"
endif() # if CMAKE_BUILD_TYPE == "Debug"

set(wxWidgets_LIBRARIES
	${wxWidgets_LIBRARIES}
	comctl32 Rpcrt4
)

# I hate doing this, but wxWidgets produces way too many warnings in this category and obscures problems in my code.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

