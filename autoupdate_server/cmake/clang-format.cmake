# Originally taken from https://arcanis.me/en/2015/10/17/cppcheck-and-clang-format
# additional target to perform clang-format run, requires clang-format
# Some additions have been made to better suit cszb-scoreboard's usage

file(GLOB PROTO_FILES "*.proto")
file(GLOB_RECURSE MAIN_SOURCE_FILES src/*.cpp include/*.h)
file(GLOB_RECURSE TEST_SOURCE_FILES test/*.cpp test/*.h)

set(ALL_SOURCE_FILES ${MAIN_SOURCE_FILES} ${TEST_SOURCE_FILES} ${PROTO_FILES})

add_custom_target(
	clangformat
	COMMAND /usr/bin/clang-format
	-style=file
	-i
	${ALL_SOURCE_FILES}
)

