# CMakeList.txt : CMake project for cszb-scoreboard
# 
# Copyright 2019-2020 Tracy Beck
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

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/VcpkgCMakeUtils.cmake ${Protobuf_INCLUDE_DIR})

SET(PROJECT_NAME "cszb-scoreboard")
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)
set (Protobuf_BUILD_SHARED_LIBS OFF)
set (Protobuf_MSVC_STATIC_RUNTIME ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

if(MSVC)
	set(LINKING_TYPE "static")
else()
	set(LINKING_TYPE "dynamic")
endif()

find_package(Protobuf REQUIRED)
find_package(GTest REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(CURL CONFIG REQUIRED)

initialize_vcpkg_code()

if("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    add_definitions(-DwxDEBUG_LEVEL=0)
endif()

file(GLOB SCOREBOARD_PROTO_SOURCES "*.proto")
include_directories(PRIVATE "${CMAKE_SOURCE_DIR}/include")

file(GLOB SCOREBOARD_SRC
    "src/*.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
    "src/*/*/*/*.cpp"
    "src/*/*/*/*/*.cpp"
)

# Is this WIN32 bad for cross-platform?  Maybe?  It's untested
add_executable(${PROJECT_NAME} WIN32 ${SCOREBOARD_SRC} ${SCOREBOARD_PROTO_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} jsoncpp_lib CURL::libcurl)

protobuf_generate(TARGET ${PROJECT_NAME})

finalize_vcpkg_code(${PROJECT_NAME})

# Testing specific code follows

list(APPEND TEST_ONLY_SRC
    "test/GuiTest.cpp"
    "test/GuiTest.h"
)

# Taken from modern cbuild (https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html)
macro(package_add_test TESTNAME)
    # create an exectuable in which the tests will be stored.  Always include our proto files, for convenience
    add_executable(${TESTNAME} ${SCOREBOARD_PROTO_SOURCES} ${ARGN})
    target_compile_definitions(${TESTNAME} PUBLIC SCOREBOARD_TESTING)
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME} PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} jsoncpp_lib CURL::libcurl
                      ${GTEST_BOTH_LIBRARIES})
    protobuf_generate(TARGET ${TESTNAME})
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER test)
endmacro()

macro(package_add_ui_test TESTNAME)
    package_add_test(${TESTNAME} ${ARGN} ${TEST_ONLY_SRC} ${SCOREBOARD_SRC})
endmacro()


enable_testing()

package_add_test(AutoUpdateTest  test/AutoUpdateTest.cpp src/util/AutoUpdate.cpp
                 src/config/CommandArgs.cpp src/util/StringUtil.cpp)
package_add_test(ProtoUtilTest test/ProtoUtilTest.cpp src/util/ProtoUtil.cpp src/ui/graphics/Color.cpp)
package_add_ui_test(ScreenPreviewTest  test/ScreenPreviewTest.cpp)
package_add_ui_test(TextEntryTest  test/TextEntryTest.cpp)

