The swx path holds encpsulated wxWidgets classes for better
mockability/testability as well as a cleaner boundary between wx and the
scoreboard itself.
Every class in this path should:
* Have all functions declared virtual, for gMock
* Not take their parent widget as a constructor.  Parent widgets should produce
child widgets instead

eg:
The old way.
```
wxPanel *foo;
wxPanel *bar = new wxPanel(foo);
```

The new way.
```
unique_ptr<SwxPanel> foo;
unique_ptr<SwxPanel> bar = foo.ChildPanel();
```

Each SwxPanel would contain the wxPanel* internally and maintain the wxWidgets
hierarchy therein.  This has an advantage of being more cleanly mocked, but also
better expresses the parent/child relationship.
