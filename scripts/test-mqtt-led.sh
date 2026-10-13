#!/usr/bin/env bash
# Run the hardware-independent MQTT command tests without Arduino dependencies.
set -euo pipefail

if [[ "${1:-}" == "--help" ]]; then
  echo "Usage: scripts/test-mqtt-led.sh"
  echo "Build and run MQTT LED command tests using a C++ compiler (CXX or c++)."
  exit 0
fi
if [[ $# -ne 0 ]]; then
  echo "Unexpected argument. Use --help for usage." >&2
  exit 2
fi

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
compiler="${CXX:-c++}"
if ! command -v "$compiler" >/dev/null 2>&1; then
  echo "C++ compiler not found: $compiler. Install one or set CXX." >&2
  exit 1
fi
build_dir="$(mktemp -d)"
trap 'rm -rf "$build_dir"' EXIT

"$compiler" -std=c++11 -Wall -Wextra -Werror -pedantic \
  "$root/tests/MQTTLedTest.cpp" -o "$build_dir/mqtt-led-test"
"$build_dir/mqtt-led-test"
