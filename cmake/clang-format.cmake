# cmake/clang-format.cmake : CMake target for clang-format formatting
# 
# Originally taken from https://arcanis.me/en/2015/10/17/cppcheck-and-clang-format
# additional target to perform clang-format run, requires clang-format

file(GLOB PROTO_FILES "*.proto")
file(GLOB_RECURSE MAIN_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp include/*.h)
file(GLOB_RECURSE TEST_SOURCE_FILES CONFIGURE_DEPENDS test/*.cpp test/*.h)

set(ALL_SOURCE_FILES ${MAIN_SOURCE_FILES} ${TEST_SOURCE_FILES} ${PROTO_FILES})

# Find clang-format executable dynamically
find_program(CLANG_FORMAT_EXECUTABLE clang-format)
if(NOT CLANG_FORMAT_EXECUTABLE)
	# Fallback if not found in standard paths, assuming it might be in user PATH
	set(CLANG_FORMAT_EXECUTABLE "clang-format")
endif()

add_custom_target(
	clangformat
	COMMAND ${CLANG_FORMAT_EXECUTABLE}
	-style=file
	-i
	${ALL_SOURCE_FILES}
)
