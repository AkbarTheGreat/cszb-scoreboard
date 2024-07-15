#!/bin/bash

# Variables I expect here:
#   BUILD_PLATFORM -- The platform to build for (default: "Linux")
#   BUILD_PRESET   -- The build preset to use (default: "Debug")
#   BUILD_THREADS  -- The number of threads to use while building (default: "" -- guessed at by cores)
#   RUN_TESTS      -- If non-empty, run tests (default: empty/false)

# Exit on first error, for easier error tracking
set -e

# Set defaults

if [ -z ${BUILD_PLATFORM} ]; then
  BUILD_PLATFORM="Linux"
fi

if [ -z ${BUILD_PRESET} ]; then
  BUILD_PRESET="Debug"
fi

# Setup any integration dependencies (like supervisord)
if [ ${BUILD_PRESET} == "Integration" ]; then
  echo "Integration test setup..."
  supervisord -c /supervisord.conf
fi

echo "Preset is ${BUILD_PLATFORM}-${BUILD_PRESET}"

echo "Configure CMake..."
cmake --preset ${BUILD_PLATFORM}-${BUILD_PRESET}

echo "Building..."
cmake --build --preset ${BUILD_PLATFORM}-${BUILD_PRESET} --parallel ${BUILD_THREADS}

if [ -n "${RUN_TESTS}" ]; then
  echo "Build complete, running tests"
  ctest --preset ${BUILD_PLATFORM}-${BUILD_PRESET}
fi

