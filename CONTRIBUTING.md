# Contributing to Smash++

Thanks for helping make Smash++ faster and more reliable. This project is performance-sensitive, so changes should be small enough to review, but verified enough to trust.

## Development Setup

Smash++ requires CMake 4.0.0 or newer and a C++20 compiler.

```sh
cmake --preset debug
cmake --build --preset debug
```

For release-like local checks, use the strict preset:

```sh
cmake --preset strict
cmake --build --preset strict
ctest --preset strict
```

The sanitizer preset is useful for memory and undefined-behavior checks on Clang/GCC platforms:

```sh
cmake --preset sanitizers
cmake --build --preset sanitizers
ctest --preset sanitizers
```

## Tests

Run the full test suite before opening a pull request:

```sh
ctest --test-dir build/release --output-on-failure --parallel 8
```

When changing packaging, versioning, benchmarks, or normalization behavior, also run the focused tests that cover that area:

```sh
ctest --test-dir build/release -L packaging --output-on-failure
ctest --test-dir build/release -L benchmark --output-on-failure
ctest --test-dir build/release -R "normalization|invalid-base" --output-on-failure
```

## Benchmarks

For performance work, compare against the previous production executable:

```sh
cmake -S . -B build/bench \
  -DCMAKE_BUILD_TYPE=Release \
  -DSMASHPP_BENCHMARK_BASELINE=/path/to/previous/smashpp
cmake --build build/bench --target smashpp-benchmark --parallel 8
```

Review both `build/bench/benchmarks/summary.csv` and `build/bench/benchmarks/comparison.csv`. Use the same compiler, build type, OpenMP setting, input sizes, and machine for both executables.

For quick benchmark-harness checks, use:

```sh
cmake --preset benchmark-smoke
cmake --build --preset benchmark-smoke
ctest --preset benchmark-smoke
```

## Code Style

- Prefer existing project patterns over new abstractions.
- Keep performance hot paths allocation-light and avoid string formatting/parsing inside per-symbol loops.
- Keep output compatibility in mind. If `.fil`, `.pos`, or `.json` output changes, document whether the change is deterministic and intentional.
- Use focused comments for non-obvious algorithmic choices, especially around sampling, filtering, and model updates.
- Do not commit generated build directories, benchmark outputs, or local `dist/` installs.

## Pull Request Checklist

Before asking for review, make sure:

- the branch builds with `SMASHPP_STRICT_WARNINGS=ON`
- relevant CTest tests pass
- benchmark changes include baseline comparison data
- output-format or compatibility changes are documented
- release or packaging changes update `docs/RELEASING.md` when needed

