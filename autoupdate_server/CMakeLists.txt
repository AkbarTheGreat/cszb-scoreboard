# CMakeList.txt : CMake project for cszb-scoreboard autoupdater
# 
# Copyright 2023 Tracy Beck
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
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/clang-format.cmake)

SET(PROJECT_NAME "cszb-scoreboard-updater")
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 20)
set(Protobuf_BUILD_SHARED_LIBS OFF)
set(Protobuf_MSVC_STATIC_RUNTIME ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(SKIP_LINT false CACHE BOOL "Skip Lint")
set(CLANG_TIDY_ERRORS true CACHE BOOL "Lint warnings fail build")
set(INTEGRATION_TEST false CACHE BOOL "Run Full Integration Tests")

find_package(absl REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads)
find_package(GTest REQUIRED CONFIG)
include(GoogleTest)

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

file(GLOB UPDATER_SRC
    "src/updater.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
    "src/*/*/*/*.cpp"
    "src/*/*/*/*/*.cpp"
)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

# Protobuf files.
file(GLOB UPDATER_PROTO_SOURCES "*.proto")
add_library(updater_proto OBJECT ${UPDATER_PROTO_SOURCES})
set_target_properties(updater_proto PROPERTIES CXX_CLANG_TIDY "")
protobuf_generate(TARGET updater_proto LANGUAGE cpp)
protobuf_generate(TARGET updater_proto LANGUAGE grpc
    GENERATE_EXTENSIONS
        .grpc.pb.h
        .grpc.pb.cc
    PLUGIN
        "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)
target_link_libraries(updater_proto PRIVATE)


add_library(updater_classes OBJECT ${UPDATER_SRC})
set_target_properties(updater_classes PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_link_libraries(updater_classes PRIVATE ${Protobuf_LIBRARIES})

add_executable(${PROJECT_NAME} WIN32 $<TARGET_OBJECTS:updater_classes> $<TARGET_OBJECTS:updater_proto>)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY "${LINT_OPTION}")
target_link_libraries(${PROJECT_NAME} PRIVATE 
        absl::base
        absl::log
        absl::strings
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)


# Testing specific code follows.

# Taken from modern cbuild (https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html)
function(package_add_test TESTNAME HAS_OWN_MAIN)
    # create an exectuable in which the tests will be stored.  Always include our proto files, for convenience
    add_executable(${TESTNAME} $<TARGET_OBJECTS:updater_proto> ${ARGN})
    target_compile_definitions(${TESTNAME} PUBLIC UPDATER_TESTING)
    if (INTEGRATION_TEST)
        target_compile_definitions(${TESTNAME} PUBLIC UPDATER_INTEGRATION_TEST)
    endif()
    if (HAS_OWN_MAIN)
        set(GTEST_LIBS ${GTEST_LIBRARIES} GTest::gmock)
    else()
        set(GTEST_LIBS ${GTEST_BOTH_LIBRARIES} GTest::gmock GTest::gmock_main)
    endif()
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME} PRIVATE ${wxWidgets_LIBRARIES}
                      ${Protobuf_LIBRARIES} ${GTEST_LIBS})
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER test CXX_CLANG_TIDY "${LINT_OPTION}")
endfunction()

enable_testing()

# config/ tests
#package_add_test(CommandArgsTest   FALSE test/unit/config/CommandArgsTest.cpp
#                                          src/config/CommandArgs.cpp)

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

