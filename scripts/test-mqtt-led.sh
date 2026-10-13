#!/usr/bin/env bash
# Run hardware-independent MQTT command and configuration tests.
set -euo pipefail

if [[ "${1:-}" == "--help" ]]; then
  echo "Usage: scripts/test-mqtt-led.sh"
  echo "Run MQTT LED command and configuration tests (requires C++ and Node.js)."
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
node --test "$root/tests/MQTTConfigTest.mjs"
