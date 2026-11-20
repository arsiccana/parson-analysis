# parson-analysis

Software verification analysis of the [parson](https://github.com/kgabis/parson) JSON library,
done as a seminar project for the Software Verification course at the Faculty of Mathematics, University of Belgrade.

## Author

- Ana Arsic 1100/2026

## Analyzed project

**parson** is a lightweight JSON library written in C. It parses JSON text into an in-memory
tree of values (objects, arrays, strings, numbers, booleans, null) and serializes that tree back to text.

- Source code: https://github.com/kgabis/parson
- Branch: `master`
- Commit: `ec53fb6528b45811df9db0db22cab96a94a96a11`

The project is included in this repository as a git submodule in the `parson/` directory.

## Cloning

```bash
git clone --recurse-submodules https://github.com/arsiccana/parson-analysis.git
```

## Tools used

| Tool | Directory | Description |
|------|-----------|-------------|
| Unit tests(Criterion) + lcov | `unit_tests/` | 	30 unit tests for parsing, building/modifying values and serialization, with line, function and branch coverage measured by lcov |
| Valgrind Memcheck | `valgrind/` | TODO |
| Cppcheck | `cppcheck/` | TODO |

## Reproducing the results

Tested on Ubuntu 22.04 with gcc 13.

### Unit tests and coverage

Requirements:

```bash
sudo apt install build-essential libcriterion-dev lcov
```

Run:

```bash
./unit_tests/run_tests.sh
```

The script compiles `parson.c` with `--coverage`, compiles the tests from `unit_tests/tests/`,
runs them and generates the coverage report. Results are written to `unit_tests/coverage/`:

- `test_output.txt` - output of the test run
- `summary.txt` - coverage summary
- `html/index.html` - detailed HTML coverage report

To run a single test or group after the script has been run once:

```bash
./unit_tests/build/unit_tests --filter 'parse/number' --verbose
```

### Valgrind Memcheck

TODO

### Cppcheck

TODO


## Conclusions

TODO
