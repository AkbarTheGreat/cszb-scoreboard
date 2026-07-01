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
