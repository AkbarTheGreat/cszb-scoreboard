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

if("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    add_definitions(-DwxDEBUG_LEVEL=0)
endif()


include_directories(PRIVATE "${CMAKE_SOURCE_DIR}/include")

file(GLOB SCOREBOARD_PROTO "proto/*.proto")

file(GLOB SCOREBOARD_SRC
    "include/*.h"
    "include/*/*.h"
    "include/*/*/*.h"
    "src/*.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
)

# Is this WIN32 bad for cross-platform?  Maybe?  It's untested
add_executable(${PROJECT_NAME} WIN32 ${SCOREBOARD_SRC} ${SCOREBOARD_PROTO})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${protobuf_LIBRARIES})

protobuf_generate(TARGET ${PROJECT_NAME})

finalize_vcpkg_code(${PROJECT_NAME})

# Testing specific code follows

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
  GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

find_package(GTest)

list(APPEND TEST_ONLY_SRC
    "test/GuiTest.cpp"
    "test/GuiTest.h"
)

# Taken from modern cbuild (https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html)
macro(package_add_test TESTNAME)
    # create an exectuable in which the tests will be stored.  Always include our proto files, for convenience
    add_executable(${TESTNAME} ${SCOREBOARD_PROTO} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME} ${wxWidgets_LIBRARIES} ${protobuf_LIBRARIES} gtest gmock gtest_main)
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

package_add_test(ProtoUtilTest test/ProtoUtilTest.cpp src/util/ProtoUtil.cpp src/ui/graphics/Color.cpp)
package_add_ui_test(ScreenPreviewTest  test/ScreenPreviewTest.cpp)
package_add_ui_test(TextEntryTest  test/TextEntryTest.cpp)

