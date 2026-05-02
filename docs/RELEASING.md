# Releasing Smash++

This checklist is for maintainers preparing a production release. Run commands from the repository root unless noted otherwise.

## 1. Start Clean

Confirm the branch and worktree state:

```sh
git status --short
git branch --show-current
git describe --tags --dirty --always --match "v[0-9]*" --match "[0-9]*"
```

The release branch should contain only intentional changes. Avoid tagging from a dirty worktree.

## 2. Build With Strict Warnings

Configure a fresh release build with warnings treated as errors:

```sh
cmake -S . -B build-release \
  -DCMAKE_BUILD_TYPE=Release \
  -DSMASHPP_STRICT_WARNINGS=ON
cmake --build build-release --parallel 8
```

If OpenMP availability is being tested separately, keep one strict warning build with the default options and one with `-DSMASHPP_ENABLE_OPENMP=OFF`.

## 3. Run Tests

Run the full regression suite:

```sh
ctest --test-dir build-release --output-on-failure --parallel 8
```

The expected result is all tests passing. Do not update baselines during release unless the output change is intentional and documented.

## 4. Check Packaging

Build the package artifacts and verify the installed CLI:

```sh
cmake --build build-release --target package --config Release
cmake --install build-release --prefix dist-release --config Release
dist-release/bin/smashpp --version
dist-release/bin/smashpp --help
```

On Windows, use `dist-release\bin\smashpp.exe` for the smoke checks.

## 5. Compare Performance

Compare against the previous production executable, usually the latest released `master` build:

```sh
cmake -S . -B build-bench \
  -DCMAKE_BUILD_TYPE=Release \
  -DSMASHPP_BENCHMARK_BASELINE=/path/to/previous/smashpp
cmake --build build-bench --target smashpp-benchmark --parallel 8
```

Review `build-bench/benchmarks/summary.csv`. The default large benchmark input is 256 MiB per file; override `SMASHPP_BENCHMARK_LARGE_BYTES` if a release needs a shorter smoke run or a larger stress run. Use the same machine, compiler, build type, input sizes, and OpenMP setting for both executables.

## 6. Check Output Compatibility

For default-mode changes, compare representative `.pos` or `.json` output against the previous release. If output differs, classify it before tagging:

- deterministic numeric-only differences from full-precision filtering
- expected differences from an opt-in mode such as `--approx-sampled-models`
- segment-boundary changes near thresholds
- unexpected behavioral changes

Document intentional differences in `README.md` or the release notes. Avoid silent output-format changes.

## 7. Confirm Version Metadata

Before tagging, verify the version source:

```sh
git describe --tags --dirty --always --match "v[0-9]*" --match "[0-9]*"
```

After tagging, verify the binary reports the intended release version:

```sh
git tag vX.YY
cmake -S . -B build-release-tagged -DCMAKE_BUILD_TYPE=Release
cmake --build build-release-tagged --parallel 8
build-release-tagged/smashpp --version
```

Use the project’s existing tag style and release workflow when publishing.
