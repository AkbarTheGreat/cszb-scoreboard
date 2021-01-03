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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules") # the cmake-modules repo
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/VcpkgCMakeUtils.cmake ${Protobuf_INCLUDE_DIR})

SET(PROJECT_NAME "cszb-scoreboard")
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)
set (Protobuf_BUILD_SHARED_LIBS OFF)
set (Protobuf_MSVC_STATIC_RUNTIME ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

if(MSVC OR OSXCROSS_HOST)
    set(LINKING_TYPE "static")
    if(MSVC)
        set(JSON_CPP_LIB "jsoncpp_static")
    else()
        set(JSON_CPP_LIB "jsoncpp_lib")
    endif()
else()
    set(LINKING_TYPE "dynamic")
    set(JSON_CPP_LIB "jsoncpp_lib")
endif()

if(OSXCROSS_HOST)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/osxcross.cmake)
	set(SKIP_LINT true)
endif()

find_package(Protobuf REQUIRED)
find_package(GTest REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(CURL CONFIG REQUIRED)

initialize_vcpkg_code()

if(NOT wxWidgets_LIBRARIES)
   find_package(wxWidgets REQUIRED COMPONENTS net core base aui)
   include(${wxWidgets_USE_FILE})
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    add_definitions(-DwxDEBUG_LEVEL=0)
endif()

set(LINT_OPTION, "")

# Clang tidy piece adapted from https://gitlab.kitware.com/cmake/cmake/-/issues/18926
if(NOT SKIP_LINT)
    find_program(CLANG_TIDY_COMMAND NAMES clang-tidy)
    if(CLANG_TIDY_COMMAND)
        set(CLANG_TIDY_CHECKS "-*,google-*,modernize-*,readability-*,portability-*,performance-*")
        if(CLANG_TIDY_ERRORS)
            set(CLANG_TIDY_ERROR_CHECK ";-warnings-as-errors=*")
        endif()
	set(LINT_OPTION "${CLANG_TIDY_COMMAND};-checks=${CLANG_TIDY_CHECKS}${CLANG_TIDY_ERROR_CHECK};-header-filter=${CMAKE_SOURCE_DIR}/include/.*")
    endif() # CLANG_TIDY_COMMAND
endif() # OSXCROSS_HOST

include_directories(PRIVATE "${CMAKE_SOURCE_DIR}/include")

file(GLOB SCOREBOARD_SRC
    "src/*.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
    "src/*/*/*/*.cpp"
    "src/*/*/*/*/*.cpp"
)

IF(WIN32)
    SET(SCOREBOARD_SRC ${SCOREBOARD_SRC} "resources/scoreboard.rc")
ENDIF(WIN32)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dev_settings.cmake OPTIONAL)

# Protobuf files.
file(GLOB SCOREBOARD_PROTO_SOURCES "*.proto")
add_library(scoreboard_proto OBJECT ${SCOREBOARD_PROTO_SOURCES})
set_target_properties(scoreboard_proto PROPERTIES CXX_CLANG_TIDY "")
protobuf_generate(TARGET scoreboard_proto)
target_link_libraries(scoreboard_proto PRIVATE ${JSON_CPP_LIB})


add_library(scoreboard_classes OBJECT ${SCOREBOARD_SRC})
set_target_properties(scoreboard_classes PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_link_libraries(scoreboard_classes PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${JSON_CPP_LIB} CURL::libcurl)

# Is this WIN32 bad for cross-platform?  Maybe?  It's untested.
add_executable(${PROJECT_NAME} WIN32 $<TARGET_OBJECTS:scoreboard_classes> $<TARGET_OBJECTS:scoreboard_proto>)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")

target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${JSON_CPP_LIB} CURL::libcurl)

finalize_vcpkg_code(${PROJECT_NAME})

# Testing specific code follows.

list(APPEND TEST_ONLY_SRC
    "test/GuiTest.cpp"
    "test/TestUtil.cpp"
)

add_library(scoreboard_test_classes OBJECT ${TEST_ONLY_SRC} ${SCOREBOARD_SRC})
set_target_properties(scoreboard_test_classes PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_compile_definitions(scoreboard_test_classes PUBLIC SCOREBOARD_TESTING)
target_link_libraries(scoreboard_test_classes PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${JSON_CPP_LIB} CURL::libcurl)

# Taken from modern cbuild (https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html)
macro(package_add_test TESTNAME)
    # create an exectuable in which the tests will be stored.  Always include our proto files, for convenience
    add_executable(${TESTNAME} $<TARGET_OBJECTS:scoreboard_proto> ${ARGN})
    target_compile_definitions(${TESTNAME} PUBLIC SCOREBOARD_TESTING)
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME} PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${JSON_CPP_LIB} CURL::libcurl
                      ${GTEST_BOTH_LIBRARIES})
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER test CXX_CLANG_TIDY "${LINT_OPTION}")
endmacro()

macro(package_add_ui_test TESTNAME)
	package_add_test(${TESTNAME} ${ARGN} $<TARGET_OBJECTS:scoreboard_test_classes> )
endmacro()


enable_testing()

package_add_test(AutoUpdateTest  test/AutoUpdateTest.cpp src/util/AutoUpdate.cpp
	src/config/CommandArgs.cpp src/util/StringUtil.cpp)
package_add_test(ImageLibraryTest  test/ImageLibraryTest.cpp src src/config/ImageLibrary.cpp
                 src/config/CommandArgs.cpp
                 src/config/Persistence.cpp
                 src/util/FilesystemPath.cpp
                 )
package_add_test(ProtoUtilTest test/ProtoUtilTest.cpp src/util/ProtoUtil.cpp src/ui/graphics/Color.cpp)
package_add_ui_test(ScreenPreviewTest  test/ScreenPreviewTest.cpp)
package_add_ui_test(TextEntryTest  test/TextEntryTest.cpp)
package_add_ui_test(ScreenTextSideTest  test/ScreenTextSideTest.cpp)
 
if (ENABLE_CODE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    file(RELATIVE_PATH BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    list(APPEND COVERAGE_EXCLUDES "${BUILD_DIR}/*")
    list(APPEND COVERAGE_EXCLUDES "out/*")
    setup_target_for_coverage_gcovr_html(
	    NAME ${PROJECT_NAME}-coverage
	    EXECUTABLE ctest
	    BASE_DIRECTORY "${PROJECT_DIR}/src" 
	    EXCLUDE ${COVERAGE_EXCLUDES}
	    )
    setup_target_for_coverage_gcovr_xml(
	    NAME ${PROJECT_NAME}-xml-coverage
	    EXECUTABLE ctest
	    BASE_DIRECTORY "${PROJECT_DIR}/src" 
	    EXCLUDE ${COVERAGE_EXCLUDES}
	    )
endif()

