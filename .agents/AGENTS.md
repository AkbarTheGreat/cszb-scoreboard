# Project Rules

- The best way to test code changes is to build affected platforms using native windows builds or
  podman containers with a volume mount:

  ### Windows (Native Build & Test)

  ```bash
  cmake --preset Win64-Integration
  cmake --build --preset Win64-Integration --parallel 10
  ctest --preset Win64-Integration
  ```

  ### Linux (Container Build & Test)

  - Container image: `localhost/scoreboard` (target: `standard_test`)

  ```bash
  podman run --rm -v s:\src\cszb-scoreboard:/cszb-scoreboard -e RUN_TESTS=yes -e BUILD_THREADS=10 localhost/scoreboard
  ```

  ### MacOS (osxcross Container Build & Test)

  - Container image: `localhost/osxcross` (target: `macos_test`)

  ```bash
  podman run --rm -v s:\src\cszb-scoreboard:/cszb-scoreboard -e RUN_TESTS=yes -e BUILD_THREADS=10 localhost/osxcross
  ```

- **Static Analysis & Linting (Strict Checks)**:

  - The Linux container build (`localhost/scoreboard`) runs static analysis (`clang-tidy`) and
    treats warnings as errors (`-warnings-as-errors`).
  - Always run the container build to ensure new code satisfies checks such as
    `modernize-use-nodiscard`, `readability-implicit-bool-conversion`, `readability-magic-numbers`,
    `modernize-use-auto`, and `readability-convert-member-functions-to-static`. Passing the local
    MSVC Windows build is not sufficient alone.

- **Targeted Integration/Unit Testing**:

  - Running all 154+ tests can be slow. Run subset matching to speed up iteration (e.g. for
    `ThingsMode` panels):
    ```bash
    ctest --preset Win64-Integration -R ThingsMode
    ```

- **wxWidgets Wrapper Idioms**:

  - Avoid calling `.wx()` from outside layout or widget classes. Implement helper methods on
    `Widget` (e.g., `relativeVerticalCenter`, `backgroundColor`) to encapsulate positioning and
    layout queries.
  - Utilize lightweight wrappers like `Image` (subclassed from `wxImage`) to avoid direct, bare
    dependencies on `wxMemoryDC`, `wxBrush`, or `wxNullBitmap` inside widget classes.
  - Declare wrapper methods as `virtual` on the `swx::` namespace classes (e.g., `swx::Button`) to
    enable proper mocking inside the test suite.
