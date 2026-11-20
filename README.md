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
| Unit tests + lcov | `unit_tests/` | TODO |
| Valgrind Memcheck | `valgrind/` | TODO |
| Cppcheck | `cppcheck/` | TODO |

Instructions for reproducing the results of each tool are in its directory.

## Conclusions

TODO
