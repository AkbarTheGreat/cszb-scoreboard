# Sourced from https://github.com/asjadenet/wxhellovcpkg/blob/master/winmsvc.cmake on 23-11-2019.
# Licensed via the MIT license, Copyright (c) 2018 Tiit Ülejõe

# Modifications made later by Tracy Beck for cszb-scoreboard, under the Apache license.

if(NOT LINKING_TYPE)
    message(FATAL_ERROR "Please specify -DLINKING_TYPE=static or -DLINKING_TYPE=dynamic")
    return()
endif()

if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Please specify CMAKE_TOOLCHAIN_FILE, for example -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake")
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

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
	add_definitions(-D_DEBUG)
	include_directories(SYSTEM INTERFACE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib/mswud ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
	set(wxWidgets_LIB_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug)
	if(LINKING_TYPE MATCHES "static")
		set(wxWidgets_LIBRARIES 
			${wxWidgets_LIB_DIR}/lib/wxbase31ud_net.lib
			${wxWidgets_LIB_DIR}/lib/wxmsw31ud_aui.lib
			${wxWidgets_LIB_DIR}/lib/wxmsw31ud_core.lib
			${wxWidgets_LIB_DIR}/lib/wxbase31ud.lib
			${wxWidgets_LIB_DIR}/lib/wxregexud.lib
			${wxWidgets_LIB_DIR}/lib/jpeg.lib
			${wxWidgets_LIB_DIR}/lib/libpng16d.lib
			${wxWidgets_LIB_DIR}/lib/lzmad.lib
			${wxWidgets_LIB_DIR}/lib/tiffd.lib
			${wxWidgets_LIB_DIR}/lib/zlibd.lib
		)
	else()
		set(wxWidgets_LIBRARIES
			${wxWidgets_LIB_DIR}/bin/wxbase313ud_net_vc_x64_custom.dll
			${wxWidgets_LIB_DIR}/bin/wxmsw313ud_aui_vc_x64_custom.dll
			${wxWidgets_LIB_DIR}/bin/wxmsw313ud_core_vc_x64_custom.dll
			${wxWidgets_LIB_DIR}/bin/wxbase313ud_vc_x64_custom.dll
			${wxWidgets_LIB_DIR}/bin/libpng16d.dll
			${wxWidgets_LIB_DIR}/bin/zlibd1.dll
		)
	endif()
	set(wxWidgets_LIBRARIES
  		${wxWidgets_LIBRARIES}
        comctl32 Rpcrt4
		)
elseif("${CMAKE_BUILD_TYPE}" MATCHES "Release")
	include_directories(SYSTEM INTERFACE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/mswu ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
	set(wxWidgets_LIB_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})
	if(LINKING_TYPE MATCHES "static")
		set(wxWidgets_LIBRARIES 
			${wxWidgets_LIB_DIR}/lib/wxbase31u_net.lib
			${wxWidgets_LIB_DIR}/lib/wxmsw31u_aui.lib
			${wxWidgets_LIB_DIR}/lib/wxmsw31u_core.lib
			${wxWidgets_LIB_DIR}/lib/wxbase31u.lib
			${wxWidgets_LIB_DIR}/lib/wxregexu.lib
			${wxWidgets_LIB_DIR}/lib/jpeg.lib
			${wxWidgets_LIB_DIR}/lib/libpng16.lib
			${wxWidgets_LIB_DIR}/lib/lzma.lib
			${wxWidgets_LIB_DIR}/lib/tiff.lib
			${wxWidgets_LIB_DIR}/lib/zlib.lib
		)
	else()
		set(wxWidgets_LIBRARIES
			${wxWidgets_LIB_DIR}/bin/wxbase313u_net_vc_x64_custom.dll
			${wxWidgets_LIB_DIR}/bin/wxmsw313u_aui_vc_x64_custom.dll
			${wxWidgets_LIB_DIR}/bin/wxmsw313u_core_vc_x64_custom.dll
			${wxWidgets_LIB_DIR}/bin/wxbase313u_vc_x64_custom.dll
			${wxWidgets_LIB_DIR}/bin/libpng16.dll
			${wxWidgets_LIB_DIR}/bin/zlib1.dll
		)
	endif()
	set(wxWidgets_LIBRARIES
  		${wxWidgets_LIBRARIES}
        comctl32 Rpcrt4
		)
endif()