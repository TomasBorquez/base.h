#!/usr/bin/env bash
set -u

TESTS=(
  "arena-tests"
  "file-system-tests"
  "ini-parser-tests"
  "string-tests"
  "vector-tests"
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

  if ! "$COMPILER" -g -O0 "${test}.c" -o "${test}${EXE}" -lm; then
    echo "Compilation of $test failed"
    cleanup
    exit 1
  fi

  if ! valgrind --leak-check=full --show-leak-kinds=all ./"${test}${EXE}"; then
    echo "$test failed with $COMPILER"
    cleanup
    exit 1
  fi

  smatch_result=$(smatch "${test}.c")
  if [ ${#smatch_result} -gt 1 ]; then
    echo "Smatch found an error on $test: "
    echo "$smatch_result"
    cleanup
    exit 1
  fi
done

echo "All tests passed with $COMPILER"
cleanup
