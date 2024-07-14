#!/bin/bash

# Exit on first error, for easier error tracking
set -e

if [ ${BUILD_PRESET} == "Integration" ]; then
  echo "Integration test setup..."
  supervisord -c /supervisord.conf
fi

echo "Configure CMake..."
cmake --preset Linux-${BUILD_PRESET}

echo "Building..."
cmake --build --preset Linux-${BUILD_PRESET} --parallel ${BUILD_THREADS}

if [ -n "${RUN_TESTS}" ]; then
  echo "Build complete, running tests"
  ctest --preset Linux-${BUILD_PRESET}
fi

