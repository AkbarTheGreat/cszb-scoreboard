# CMakeList.txt : CMake project for cszb-scoreboard
# 
# Copyright 2019-2026 Tracy Beck
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

cmake_minimum_required(VERSION 3.12.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dev_settings.cmake OPTIONAL)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules") # the cmake-modules repo
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/VcpkgCMakeUtils.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/clang-format.cmake)

# C was finding its way into enabled languages (likely due to submodules), so we
# explicitly set it here.  Otherwise, coverage breaks due to incomplete compiler
# configuration.
set(PROJECT_NAME "cszb-scoreboard")
project(${PROJECT_NAME} C CXX)

set(CMAKE_CXX_STANDARD 20)
set(Protobuf_BUILD_SHARED_LIBS OFF)
set(Protobuf_MSVC_STATIC_RUNTIME ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(JSON_CPP_LIB "jsoncpp_lib")

set(LINKING_TYPE "static" CACHE STRING "Link type")
set(SKIP_LINT false CACHE BOOL "Skip Lint")
set(CLANG_TIDY_ERRORS true CACHE BOOL "Lint warnings fail build")
set(INTEGRATION_TEST false CACHE BOOL "Run Full Integration Tests")

set(PROTOC_EXECUTABLE "protobuf::protoc")

# Common wxWidgets settings
set(wxBUILD_MONOLITHIC "OFF")
set(wxBUILD_SAMPLES "OFF")
set(wxBUILD_SHARED "OFF")
set(wxUSE_LIBWEBP "OFF")
set(wxUSE_STC "OFF")
set(wxUSE_WEBVIEW_IE "OFF")
set(wxUSE_WEBVIEW_WEBKIT "ON")

set(CURL_DISABLE_SSH ON CACHE INTERNAL "Disable SSH support")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build static libcurl library")
set(CURL_STATICLIB ON CACHE INTERNAL "Use static libcurl")
set(CURL_USE_OPENSSL ON CACHE INTERNAL "")
set(CURL_USE_LIBPSL OFF CACHE INTERNAL "Disable Public Suffix List")

if(MSVC OR OSXCROSS_HOST)
    set(LINKING_TYPE "static")
    if(MSVC)
        set(JSON_CPP_LIB "jsoncpp_static")
    else()
        set(JSON_CPP_LIB "jsoncpp_lib")
    endif()
endif()

if(OSXCROSS_HOST)
    message("OSXCROSS_HOST is set")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/osxcross.cmake)
	set(SKIP_LINT true)
endif()

find_package(absl REQUIRED)
find_package(Protobuf REQUIRED)
find_package(GTest REQUIRED CONFIG)
include(GoogleTest)
find_package(jsoncpp REQUIRED)

initialize_vcpkg_code()

if("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    add_compile_definitions(NDEBUG wxDEBUG_LEVEL=0)
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    add_compile_definitions(SCOREBOARD_DEBUG _DEBUG wxDEBUG_LEVEL=1)
endif()

add_subdirectory(thirdparty/wxWidgets)
add_subdirectory(thirdparty/curl)

set(LINT_OPTION "")

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
include_directories(PUBLIC
    "${CMAKE_SOURCE_DIR}/thirdparty/curl/include"
    "${CMAKE_SOURCE_DIR}/thirdparty/wxWidgets/include"
  )

# Gather all source files recursively using CONFIGURE_DEPENDS
file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS "src/*.cpp")
list(REMOVE_ITEM CORE_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cszb-scoreboard.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dialog-test.cpp"
)

set(BASE_LIBRARIES
	absl::log_internal_check_op
	absl::log_internal_conditions
	absl::log_internal_message
	absl::raw_hash_set
	absl::statusor
    libcurl
    wxWidgets
    ${wxWidgets_LIBRARIES}
    ${Protobuf_LIBRARIES}
    ${JSON_CPP_LIB}
)

if(WIN32)
    list(APPEND BASE_LIBRARIES utf8_range::utf8_validity)
endif()

# Protobuf files.
file(GLOB SCOREBOARD_PROTO_SOURCES "*.proto")
add_library(scoreboard_proto STATIC ${SCOREBOARD_PROTO_SOURCES})
set_target_properties(scoreboard_proto PROPERTIES CXX_CLANG_TIDY "")
protobuf_generate(TARGET scoreboard_proto PROTOC_EXE ${PROTOC_EXECUTABLE})
target_link_libraries(scoreboard_proto PUBLIC ${JSON_CPP_LIB} ${Protobuf_LIBRARIES})

# By setting IWYU here, we don't run IWYU on thirdparty or protobuf targets, which we don't want to modify.
if(SCOREBOARD_IWYU)
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${SCOREBOARD_IWYU})
endif()

# Core application logic static library
add_library(scoreboard_core STATIC ${CORE_SRC})
add_dependencies(scoreboard_core scoreboard_proto)
set_target_properties(scoreboard_core PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_link_libraries(scoreboard_core PUBLIC ${BASE_LIBRARIES} scoreboard_proto)

# Core library with test definitions enabled (includes cszb-scoreboard.cpp for tests)
add_library(scoreboard_core_testing STATIC ${CORE_SRC} src/cszb-scoreboard.cpp)
add_dependencies(scoreboard_core_testing scoreboard_proto)
set_target_properties(scoreboard_core_testing PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_compile_definitions(scoreboard_core_testing PUBLIC
    SCOREBOARD_TESTING
    SCOREBOARD_DISABLE_IMAGE_SEARCH
)
target_link_libraries(scoreboard_core_testing PUBLIC ${BASE_LIBRARIES} scoreboard_proto)

# Core library with UI test definitions enabled (includes cszb-scoreboard.cpp and UI test macros)
add_library(scoreboard_core_ui_testing STATIC ${CORE_SRC} src/cszb-scoreboard.cpp)
add_dependencies(scoreboard_core_ui_testing scoreboard_proto)
set_target_properties(scoreboard_core_ui_testing PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_compile_definitions(scoreboard_core_ui_testing PUBLIC
    SCOREBOARD_TESTING
    SCOREBOARD_DISABLE_IMAGE_SEARCH
    SCOREBOARD_UI_TEST
)
target_link_libraries(scoreboard_core_ui_testing PUBLIC ${BASE_LIBRARIES} scoreboard_proto)

# Main Executable
add_executable(${PROJECT_NAME} WIN32 src/cszb-scoreboard.cpp)
if(WIN32)
    target_sources(${PROJECT_NAME} PRIVATE "resources/scoreboard.rc")
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_link_libraries(${PROJECT_NAME} PRIVATE scoreboard_core)

# A test target which is used for checking dialog edits more quickly than running entire application.
add_executable("dialog-test" WIN32 src/dialog-test.cpp)
set_target_properties("dialog-test" PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_compile_definitions(dialog-test PUBLIC FAKE_CONFIGURATION_FILES)
target_link_libraries("dialog-test" PRIVATE scoreboard_core)

finalize_vcpkg_code(${PROJECT_NAME})

# Testing specific code follows.
enable_testing()

# Shared UI Test base library
add_library(scoreboard_ui_test_base STATIC test/integration/GuiTest.cpp)
target_link_libraries(scoreboard_ui_test_base PUBLIC scoreboard_core_ui_testing)

# Taken from modern cbuild (https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html)
function(package_add_test_impl TESTNAME HAS_OWN_MAIN CORE_LIB)
    # create an executable in which the tests will be stored.
    add_executable(${TESTNAME} ${ARGN})
    target_compile_definitions(${TESTNAME} PUBLIC SCOREBOARD_TESTING)
    if (INTEGRATION_TEST)
        target_compile_definitions(${TESTNAME} PUBLIC SCOREBOARD_INTEGRATION_TEST)
    endif()
    if (HAS_OWN_MAIN)
        set(GTEST_LIBS ${GTEST_LIBRARIES} GTest::gmock)
    else()
        set(GTEST_LIBS ${GTEST_BOTH_LIBRARIES} GTest::gmock GTest::gmock_main)
    endif()

    target_link_libraries(${TESTNAME} PRIVATE ${CORE_LIB} ${GTEST_LIBS})

    gtest_discover_tests(${TESTNAME}
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
        XML_OUTPUT_DIR gtest
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER test CXX_CLANG_TIDY "${LINT_OPTION}")
endfunction()

function(package_add_test TESTNAME HAS_OWN_MAIN)
    package_add_test_impl(${TESTNAME} ${HAS_OWN_MAIN} scoreboard_core_testing ${ARGN})
endfunction()

function(package_add_ui_test TESTNAME HAS_OWN_MAIN)
    if (INTEGRATION_TEST)
	    package_add_test_impl(${TESTNAME} ${HAS_OWN_MAIN} scoreboard_core_ui_testing ${ARGN})
        target_link_libraries(${TESTNAME} PRIVATE scoreboard_ui_test_base)
    endif()
endfunction()

function(package_add_macos_test TESTNAME HAS_OWN_MAIN)
    package_add_test(${TESTNAME}_MacOS ${HAS_OWN_MAIN} ${ARGN})
    target_compile_definitions(${TESTNAME}_MacOS PUBLIC SCOREBOARD_APPLE_IMPL)
endfunction()

# meta/ tests
package_add_test(TempFilesystemTest      FALSE test/unit/meta/TempFilesystemTest.cpp)

# config/ tests
package_add_test(CommandArgsTest         FALSE test/unit/config/CommandArgsTest.cpp)
package_add_test(DisplayConfigTest       FALSE test/unit/config/DisplayConfigTest.cpp)
package_add_test(GeneralConfigTest       FALSE test/unit/config/GeneralConfigTest.cpp)
package_add_test(ImageLibraryTest        FALSE test/unit/config/ImageLibraryTest.cpp)
package_add_test(CaseOptionalStringTest  FALSE test/unit/config/ImageLibrary/CaseOptionalStringTest.cpp)

# util/ tests
package_add_test(AutoUpdateTest          FALSE test/unit/util/AutoUpdateTest.cpp)
package_add_test(Base64Test              FALSE test/unit/util/Base64Test.cpp)
package_add_test(FilesystemPathTest      TRUE  test/unit/util/FilesystemPathTest.cpp
                                                src/util/FilesystemPath.cpp)
package_add_test(ProtoUtilTest           FALSE test/unit/util/ProtoUtilTest.cpp)
package_add_test(StringUtilTest          FALSE test/unit/util/StringUtilTest.cpp)

# ui/event tests
package_add_test(UpdateTimerTest         FALSE test/unit/ui/event/UpdateTimerTest.cpp)
package_add_test(LibraryScanTimerTest    FALSE test/unit/ui/event/LibraryScanTimerTest.cpp)

# integration tests
package_add_ui_test(ScreenPreviewTest    FALSE test/integration/ScreenPreviewTest.cpp)
package_add_ui_test(TextEntryTest        FALSE test/integration/TextEntryTest.cpp)
package_add_ui_test(ScreenTextSideTest   FALSE test/integration/ScreenTextSideTest.cpp)

# Tests here are Mac-specific builds of other tests.
package_add_macos_test(FilesystemPathTest  TRUE  test/unit/util/FilesystemPathTest.cpp
                                                 src/util/FilesystemPath.cpp)
 
if (ENABLE_CODE_COVERAGE)
    include(CodeCoverage)
    set(EXTRA_GCOVR_ARGS "--merge-mode-functions=separate")
    append_coverage_compiler_flags()
    file(RELATIVE_PATH BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    list(APPEND COVERAGE_EXCLUDES "${BUILD_DIR}/*")
    list(APPEND COVERAGE_EXCLUDES "out/*")
    list(APPEND COVERAGE_EXCLUDES "thirdparty/*")
    setup_target_for_coverage_gcovr_html(
	    NAME ${PROJECT_NAME}-coverage
	    EXECUTABLE ctest
	    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src" 
	    EXCLUDE ${COVERAGE_EXCLUDES}
	    )
    setup_target_for_coverage_gcovr_xml(
	    NAME ${PROJECT_NAME}-xml-coverage
	    EXECUTABLE ctest
	    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src" 
	    EXCLUDE ${COVERAGE_EXCLUDES}
	    )
endif()
