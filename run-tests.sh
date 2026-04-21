#!/usr/bin/env bash
set -u

TESTS=(
  "arena-tests"
  "string-tests"
  "vector-tests"
  "ini-parser-tests"
  "file-system-tests"
)

if [ $# -lt 1 ]; then
  echo "Usage: $0 <compiler> [specific_test]"
  echo "  compiler: gcc, clang, or tcc"
  echo "  specific_test: Optional - Run only this test file"
  exit 1
fi

COMPILER=$1
SPECIFIC_TEST="${2:-}"

case "$OSTYPE" in
  msys*|cygwin*|win32*) EXE=".exe" ;;
  *) EXE="" ;;
esac

if ! command -v $COMPILER &> /dev/null; then
  echo "Error: $COMPILER is not installed or not in PATH"
  exit 1
fi

cd ./tests/

cleanup() {
  for test in "${TESTS[@]}"; do
    rm -f "${test}${EXE}"
  done
  rm -rf "./file-system-dir/"
  echo "Cleanup complete"
}

if [ -n "$SPECIFIC_TEST" ]; then
  TESTS=("$SPECIFIC_TEST")
fi

for test in "${TESTS[@]}"; do
  echo "Running $test with $COMPILER..."

  FLAGS="-Wall -Wextra -Wpedantic -g3 -fsanitize=address,undefined"
  EXTRA_FLAGS="-Wno-unused-function -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wshadow -Wstrict-prototypes -Wnull-dereference -Wformat=2"
  FANALYZER_FLAGS="-fanalyzer -fanalyzer-call-summaries --param=analyzer-max-recursion-depth=2 --param=analyzer-max-infeasible-edges=2"

  if ! "$COMPILER" $FLAGS $EXTRA_FLAGS "${test}.c" -o "${test}${EXE}" -lm; then
    echo "Compilation of $test failed"
    cleanup
    exit 1
  fi

  if ! ./"${test}${EXE}"; then
    echo "$test failed with $COMPILER"
    cleanup
    exit 1
  fi
done

echo "All tests passed with $COMPILER"
cleanup
