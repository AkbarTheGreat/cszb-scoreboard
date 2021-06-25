The swx path holds wrappers for wx widgets which add virtual qualifiers for
some methods for mockability/testability.  If a method needs to be mocked,
add it here as a virtual method if necessary.  Otherwise, these should not be
referenced outside of the tests or the widgets path.

