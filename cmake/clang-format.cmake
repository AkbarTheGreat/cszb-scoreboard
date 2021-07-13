# Taken from https://arcanis.me/en/2015/10/17/cppcheck-and-clang-format
# additional target to perform clang-format run, requires clang-format

# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

add_custom_target(
	clangformat
	COMMAND /usr/bin/clang-format
	-style=file
	-i
	${ALL_SOURCE_FILES}
)

