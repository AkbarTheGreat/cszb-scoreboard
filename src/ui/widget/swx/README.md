The swx path holds wrappers for wx widgets which add virtual qualifiers for some methods for
mockability/testability. If a method needs to be mocked, add it here as a virtual method if
necessary. Otherwise, these should not be referenced outside of the tests or the widgets path.

The src/ui/widget/swx path should be extremely limited in scope. At present, there only exists
WebView.cpp, as that one class is special in comparison to the other light-weight swx wrappers. Do
not add things here willy-nilly, it should be kept to the absolute minimum of contents.
