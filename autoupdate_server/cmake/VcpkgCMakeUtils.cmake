# VcpkgCMakeUtils.cmake : Utility methods for making libraries from Vcpkg
# work correctly with Cmake, to allow for proper compilation on Windows,
# while not sacrificing other platform compatability.
# This WxWidget/Vcpkg functionality work was taken from
# https://github.com/asjadenet/wxhellovcpkg as a baseline, and converted
# into macros here, with the minimal changes on my part.  Originally
# licensed under the MIT license, and separated into this file to give proper
# credit where credit is due.  Additional changes made after initial commit are
# mine.

set(default_build_type "Release")

macro(initialize_vcpkg_code)
    if(NOT CMAKE_BUILD_TYPE)
      message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
      set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
          STRING "Choose the type of build." FORCE)
    endif()

    if(MSVC)
        add_definitions(/MP)
    endif()

    if(MSVC AND NOT WXSOURCE)
        include(${CMAKE_SOURCE_DIR}/cmake/winmsvc.cmake)
    endif()
    message(STATUS "CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE}")
endmacro(initialize_vcpkg_code)

macro(finalize_vcpkg_code binary_name)
    if(MSVC)
    add_custom_command(
        TARGET ${binary_name} 
        POST_BUILD
        COMMAND "mt.exe" -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\resources\\dpiaware.manifest\" -inputresource:\"$<TARGET_FILE:${PROJECT_NAME}>\"\;\#1 -outputresource:\"$<TARGET_FILE:${PROJECT_NAME}>\"\;\#1
        COMMENT "Adding display aware manifest..." 
    )
    endif()
endmacro(finalize_vcpkg_code)

