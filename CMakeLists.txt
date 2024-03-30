# CMakeList.txt : CMake project for cszb-scoreboard
# 
# Copyright 2019-2023 Tracy Beck
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

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dev_settings.cmake OPTIONAL)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules") # the cmake-modules repo
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/VcpkgCMakeUtils.cmake ${Protobuf_INCLUDE_DIR})
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/clang-format.cmake)

SET(PROJECT_NAME "cszb-scoreboard")
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 20)
set(Protobuf_BUILD_SHARED_LIBS OFF)
set(Protobuf_MSVC_STATIC_RUNTIME ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(JSON_CPP_LIB "jsoncpp_lib")

set(LINKING_TYPE "dynamic" CACHE STRING "Link type")
set(SKIP_LINT false CACHE BOOL "Skip Lint")
set(CLANG_TIDY_ERRORS true CACHE BOOL "Lint warnings fail build")
set(INTEGRATION_TEST false CACHE BOOL "Run Full Integration Tests")

if(MSVC OR OSXCROSS_HOST)
    set(LINKING_TYPE "static")
    if(MSVC)
        set(JSON_CPP_LIB "jsoncpp_static")
    else()
        set(JSON_CPP_LIB "jsoncpp_lib")
    endif()
endif()

if(OSXCROSS_HOST)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/osxcross.cmake)
	set(SKIP_LINT true)
endif()

find_package(Protobuf REQUIRED)
find_package(GTest REQUIRED CONFIG)
include(GoogleTest)
find_package(jsoncpp REQUIRED)
find_package(CURL CONFIG REQUIRED)

initialize_vcpkg_code()

if(NOT wxWidgets_LIBRARIES)
   find_package(wxWidgets REQUIRED COMPONENTS net core base aui webview)
   include(${wxWidgets_USE_FILE})
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    add_definitions(-DwxDEBUG_LEVEL=0)
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    add_definitions(-DSCOREBOARD_DEBUG)
endif()

set(LINT_OPTION, "")

# Clang tidy piece adapted from https://gitlab.kitware.com/cmake/cmake/-/issues/18926
if(NOT SKIP_LINT)
    find_program(CLANG_TIDY_COMMAND NAMES clang-tidy)
    if(CLANG_TIDY_COMMAND)
        set(CLANG_TIDY_CHECKS "-*,google-*,modernize-*,readability-*,portability-*,performance-*,-readability-identifier-length")
        if(CLANG_TIDY_ERRORS)
            set(CLANG_TIDY_ERROR_CHECK ";-warnings-as-errors=*")
        endif()
	set(LINT_OPTION "${CLANG_TIDY_COMMAND};-checks=${CLANG_TIDY_CHECKS}${CLANG_TIDY_ERROR_CHECK};-header-filter=${CMAKE_SOURCE_DIR}/include/.*")
    endif() # CLANG_TIDY_COMMAND
endif() # OSXCROSS_HOST

include_directories(PRIVATE "${CMAKE_SOURCE_DIR}/include")

file(GLOB SCOREBOARD_SRC
    "src/cszb-scoreboard.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
    "src/*/*/*/*.cpp"
    "src/*/*/*/*/*.cpp"
)

file(GLOB DIALOG_TEST_SRC
    "src/dialog-test.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
    "src/*/*/*/*.cpp"
    "src/*/*/*/*/*.cpp"
)

IF(WIN32)
    SET(SCOREBOARD_SRC ${SCOREBOARD_SRC} "resources/scoreboard.rc")
ENDIF(WIN32)

# Protobuf files.
file(GLOB SCOREBOARD_PROTO_SOURCES "*.proto")
add_library(scoreboard_proto OBJECT ${SCOREBOARD_PROTO_SOURCES})
set_target_properties(scoreboard_proto PROPERTIES CXX_CLANG_TIDY "")
protobuf_generate(TARGET scoreboard_proto)
target_link_libraries(scoreboard_proto PRIVATE ${JSON_CPP_LIB})


add_library(scoreboard_classes OBJECT ${SCOREBOARD_SRC})
add_dependencies(scoreboard_classes scoreboard_proto)
set_target_properties(scoreboard_classes PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_link_libraries(scoreboard_classes PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${JSON_CPP_LIB} CURL::libcurl)

add_library(dialog_test_classes OBJECT ${DIALOG_TEST_SRC})
add_dependencies(dialog_test_classes scoreboard_proto)
set_target_properties(dialog_test_classes PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_compile_definitions(dialog_test_classes PUBLIC FAKE_CONFIGURATION_FILES)
target_link_libraries(dialog_test_classes PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${JSON_CPP_LIB} CURL::libcurl)

add_executable(${PROJECT_NAME} WIN32 $<TARGET_OBJECTS:scoreboard_classes> $<TARGET_OBJECTS:scoreboard_proto>)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${JSON_CPP_LIB} CURL::libcurl)

# A test target which is used for checking dialog edits more quickly than running entire application.
add_executable("dialog-test" WIN32 $<TARGET_OBJECTS:dialog_test_classes> $<TARGET_OBJECTS:scoreboard_proto>)
target_link_libraries("dialog-test" PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${JSON_CPP_LIB} CURL::libcurl)

finalize_vcpkg_code(${PROJECT_NAME})

# Testing specific code follows.

list(APPEND TEST_ONLY_SRC
    "test/integration/GuiTest.cpp"
)

add_library(scoreboard_ui_test_classes OBJECT ${TEST_ONLY_SRC} ${SCOREBOARD_SRC})
add_dependencies(scoreboard_ui_test_classes scoreboard_proto)
set_target_properties(scoreboard_ui_test_classes PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_compile_definitions(scoreboard_ui_test_classes PUBLIC SCOREBOARD_TESTING)
target_compile_definitions(scoreboard_ui_test_classes PUBLIC SCOREBOARD_INTEGRATION_TEST)
target_compile_definitions(scoreboard_ui_test_classes PUBLIC SCOREBOARD_UI_TEST)
target_link_libraries(scoreboard_ui_test_classes PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${JSON_CPP_LIB} CURL::libcurl)

# Taken from modern cbuild (https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html)
function(package_add_test TESTNAME HAS_OWN_MAIN)
    # create an exectuable in which the tests will be stored.  Always include our proto files, for convenience
    add_executable(${TESTNAME} $<TARGET_OBJECTS:scoreboard_proto> ${ARGN})
    target_compile_definitions(${TESTNAME} PUBLIC SCOREBOARD_TESTING)
    if (INTEGRATION_TEST)
        target_compile_definitions(${TESTNAME} PUBLIC SCOREBOARD_INTEGRATION_TEST)
    endif()
    if (HAS_OWN_MAIN)
        set(GTEST_LIBS ${GTEST_LIBRARIES} GTest::gmock)
    else()
        set(GTEST_LIBS ${GTEST_BOTH_LIBRARIES} GTest::gmock GTest::gmock_main)
    endif()
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME} PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${JSON_CPP_LIB} CURL::libcurl
                      ${GTEST_LIBS})
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
        XML_OUTPUT_DIR gtest
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER test CXX_CLANG_TIDY "${LINT_OPTION}")
endfunction()

function(package_add_ui_test TESTNAME HAS_OWN_MAIN)
    if (INTEGRATION_TEST)
	    package_add_test(${TESTNAME} ${HAS_OWN_MAIN} ${ARGN} $<TARGET_OBJECTS:scoreboard_ui_test_classes> )
    endif()
endfunction()

function(package_add_macos_test TESTNAME HAS_OWN_MAIN)
    package_add_test(${TESTNAME}_MacOS ${HAS_OWN_MAIN} ${ARGN})
    target_compile_definitions(${TESTNAME}_MacOS PUBLIC SCOREBOARD_APPLE_IMPL)
endfunction()

enable_testing()

# config/ tests
package_add_test(CommandArgsTest   FALSE test/unit/config/CommandArgsTest.cpp
                                          src/config/CommandArgs.cpp)
package_add_test(DisplayConfigTest FALSE test/unit/config/DisplayConfigTest.cpp
                                          src/config/DisplayConfig.cpp
                                          src/config/Persistence.cpp
                                          src/config/Position.cpp
                                          src/ui/widget/Display.cpp
                                          src/ui/widget/Frame.cpp
                                          src/util/FontUtil.cpp
                                          src/util/ProtoUtil.cpp
)
package_add_test(ImageLibraryTest  FALSE test/unit/config/ImageLibraryTest.cpp
                                          src src/config/ImageLibrary.cpp
                                          src/config/Persistence.cpp
                                          src/util/FilesystemPath.cpp
)

# util/ tests
package_add_test(AutoUpdateTest      FALSE test/unit/util/AutoUpdateTest.cpp
                                            src/util/AutoUpdate.cpp
                                            src/config/CommandArgs.cpp
                                            src/util/Base64.cpp
                                            src/util/HttpReader.cpp
                                            src/util/StringUtil.cpp
)
package_add_test(Base64Test          FALSE test/unit/util/Base64Test.cpp
                                            src/util/Base64.cpp
)
package_add_test(FilesystemPathTest  TRUE  test/unit/util/FilesystemPathTest.cpp
                                            src/util/FilesystemPath.cpp
)
package_add_test(ProtoUtilTest       FALSE test/unit/util/ProtoUtilTest.cpp
                                            src/util/ProtoUtil.cpp
                                            src/util/FontUtil.cpp
                                            src/ui/graphics/Color.cpp
)
package_add_test(StringUtilTest      FALSE test/unit/util/StringUtilTest.cpp
                                            src/util/StringUtil.cpp
)


package_add_ui_test(ScreenPreviewTest   FALSE test/integration/ScreenPreviewTest.cpp)
package_add_ui_test(TextEntryTest       FALSE test/integration/TextEntryTest.cpp)
package_add_ui_test(ScreenTextSideTest  FALSE test/integration/ScreenTextSideTest.cpp)

# Tests here are Mac-specific builds of other tests.  This is usually to unit-test functionality that
# can be compiled elsewhere, but was written specifically for a mac.
package_add_macos_test(FilesystemPathTest  TRUE  test/unit/util/FilesystemPathTest.cpp
                                                 src/util/FilesystemPath.cpp
)
 
if (ENABLE_CODE_COVERAGE)
    include(CodeCoverage)
    set(EXTRA_GCOVR_ARGS "--merge-mode-functions=separate")
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

