# CMakeList.txt : CMake project for cszb-scoreboard
# 
# Copyright 2019 Tracy Beck
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

cmake_minimum_required(VERSION 3.9.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/VcpkgCMakeUtils.cmake ${protobuf_INCLUDE_DIR})

SET(PROJECT_NAME "cszb-scoreboard")
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)
set (protobuf_BUILD_SHARED_LIBS OFF)
set (protobuf_MSVC_STATIC_RUNTIME ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(LINKING_TYPE "static")

find_package(protobuf REQUIRED)

initialize_vcpkg_code()

#if(MSVC AND protobuf_MSVC_STATIC_RUNTIME)
#  foreach(flag_var
#      CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
#      CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
#    if(${flag_var} MATCHES "/MD")
#      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
#    endif(${flag_var} MATCHES "/MD")
#  endforeach()
#endif()

include_directories("${CMAKE_SOURCE_DIR}/include")

file(GLOB SCOREBOARD_SRC
    "include/*.h"
    "include/*/*.h"
    "src/*.cpp"
    "src/*/*.cpp"
    "proto/*.proto"
)

# Is this WIN32 bad for cross-platform?  Maybe?  It's untested
add_executable(${PROJECT_NAME} WIN32 ${SCOREBOARD_SRC})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${protobuf_LIBRARIES})

protobuf_generate(TARGET ${PROJECT_NAME})

finalize_vcpkg_code(${PROJECT_NAME})
# TODO: Add tests and install targets if needed.
