#!/bin/bash
# Builds the unit tests together with parson.c instrumented for coverage,
# runs them, and generates an lcov HTML coverage report.
#
# Usage: ./run_tests.sh            (run from anywhere)
# Requires: gcc, libcriterion-dev, lcov
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PARSON_DIR="$SCRIPT_DIR/../parson"
BUILD_DIR="$SCRIPT_DIR/build"
REPORT_DIR="$SCRIPT_DIR/coverage"

# gcov must be the same version as gcc, otherwise it cannot read the coverage
# data. Use gcov-<major> (e.g. gcov-13) if it exists, plain gcov otherwise.
GCC_MAJOR="$(gcc -dumpversion | cut -d. -f1)"
if command -v "gcov-$GCC_MAJOR" >/dev/null; then
    GCOV="gcov-$GCC_MAJOR"
else
    GCOV="gcov"
fi
echo "Using gcc $GCC_MAJOR with $GCOV"

rm -rf "$BUILD_DIR" "$REPORT_DIR"
mkdir -p "$BUILD_DIR" "$REPORT_DIR"
cd "$BUILD_DIR"

# Only parson.c is compiled with --coverage: gcc then records which of its
# lines/branches get executed (.gcno/.gcda files). Test files are not measured.
gcc -O0 -g --coverage -c "$PARSON_DIR/parson.c" -o parson.o
gcc -O0 -g -I"$PARSON_DIR" "$SCRIPT_DIR"/tests/*.c parson.o \
    -lgcov -lcriterion -o unit_tests

# Run all tests; Criterion prints a summary and returns non-zero if any test fails
./unit_tests --verbose 2>&1 | tee "$REPORT_DIR/test_output.txt"
[ "${PIPESTATUS[0]}" -eq 0 ] || { echo "Some tests failed"; exit 1; }

# Collect coverage data for parson.c
# (lcov_branch_coverage works on both lcov 1.x and 2.x; 2.x prints a deprecation warning)
lcov --capture --directory . --gcov-tool "$GCOV" --rc lcov_branch_coverage=1 --output-file parson.info

lcov --summary parson.info --rc lcov_branch_coverage=1 2>&1 | tee "$REPORT_DIR/summary.txt"
genhtml parson.info --branch-coverage --output-directory "$REPORT_DIR/html"

echo "HTML report: $REPORT_DIR/html/index.html"
