# Smash++

[![Anaconda-Server Badge](https://anaconda.org/bioconda/smashpp/badges/version.svg)](https://anaconda.org/bioconda/smashpp)
[![CI](https://github.com/smortezah/smashpp/actions/workflows/ci.yml/badge.svg)](https://github.com/smortezah/smashpp/actions/workflows/ci.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)

Smash++ is a fast utility for identifying and visualizing rearrangements in DNA sequences.

## Installation

Smash++ requires CMake `4.0.0` or newer and a compiler with C++20 support.

### Conda

```sh
conda install -y bioconda::smashpp
```

### Docker

```sh
docker pull smortezah/smashpp
docker run -it smortezah/smashpp
```

### Build From Source

```sh
git clone --depth 1 https://github.com/smortezah/smashpp.git
cd smashpp
bash install.sh
```

By default, `install.sh` builds in `./build` and installs `smashpp`, `smashpp-inv-rep`, and `exclude_N` into `./dist/bin`.

You can customize the build with environment variables:

```sh
PREFIX=/your/path BUILD_TYPE=Debug PARALLEL=16 bash install.sh
```

### Ubuntu

```sh
apt update && apt install -y git g++ python3-pip
pip3 install --user "cmake~=4.0.0"

git clone --depth 1 https://github.com/smortezah/smashpp.git
cd smashpp
bash install.sh
```

### macOS

```sh
brew install git python
pip3 install --user "cmake~=4.0.0"

git clone --depth 1 https://github.com/smortezah/smashpp.git
cd smashpp
bash install.sh
```

### Windows

Install Visual Studio 2022 Build Tools with the Desktop C++ workload, plus Python 3.

```powershell
py -m pip install --user "cmake~=4.0.0"
git clone --depth 1 https://github.com/smortezah/smashpp.git
cd smashpp
powershell -ExecutionPolicy Bypass -File .\install.ps1
```

The PowerShell installer supports the same knobs as the shell script, for example:

```powershell
powershell -ExecutionPolicy Bypass -File .\install.ps1 -BuildType Debug -Prefix .\dist
```

## Usage

If you used the default source install, run the binaries from `./dist/bin`.

```sh
./dist/bin/smashpp [OPTIONS] -r <REF_FILE> -t <TAR_FILE>
./dist/bin/smashpp viz [OPTIONS] -o <SVG_FILE> <POS_FILE>
```

For best results, keep the reference and target filenames short.

### Smash++ Options

Use `smashpp --help` to print the full CLI help.

| Option                                     | Value          | Description                                                                                                              | Default             |
| ------------------------------------------ | -------------- | ------------------------------------------------------------------------------------------------------------------------ | ------------------- |
| `-r`, `--reference`                        | `<FILE>`       | Reference file in `seq`, `FASTA`, or `FASTQ` format.                                                                     | Required            |
| `-t`, `--target`                           | `<FILE>`       | Target file in `seq`, `FASTA`, or `FASTQ` format.                                                                        | Required            |
| `-l`, `--level`                            | `<INT>`        | Compression level from `0` to `6`.                                                                                       | `3`                 |
| `-m`, `--min-segment-size`                 | `<INT>`        | Minimum segment size.                                                                                                    | `50`                |
| `-fmt`, `--format`                         | `<STRING>`     | Output format: `pos` or `json`.                                                                                          | `pos`               |
| `-e`, `--entropy-N`                        | `<FLOAT>`      | Entropy assigned to `N` bases.                                                                                           | `2.0`               |
| `-n`, `--num-threads`                      | `<INT>`        | Number of worker threads.                                                                                                | `4`                 |
| `-mem`, `--max-memory`                     | `<SIZE>`       | Maximum estimated memory use. Supports `B`, `K`, `M`, `G`, and `T` suffixes; `0` disables the check.                     | Auto                |
| `-f`, `--filter-size`                      | `<INT>`        | Filter window size.                                                                                                      | `100`               |
| `-ft`, `--filter-type`                     | `<INT/STRING>` | Window function: `0/rectangular`, `1/hamming`, `2/hann`, `3/blackman`, `4/triangular`, `5/welch`, `6/sine`, `7/nuttall`. | `hann`              |
| `-fs`, `--filter-scale`                    | `<STRING>`     | Filter scale: `S/small`, `M/medium`, or `L/large`.                                                                       | Auto                |
| `-d`, `--sampling-step`                    | `<INT>`        | Sampling step.                                                                                                           | Auto                |
| `--approx-sampled-models`                  | `-`            | Use faster approximate updates between sampled positions in multi-model runs.                                            | Disabled            |
| `-th`, `--threshold`                       | `<FLOAT>`      | Segmentation threshold.                                                                                                  | `1.5`               |
| `-rb`, `--reference-begin-guard`           | `<INT>`        | Reference begin guard.                                                                                                   | `0`                 |
| `-re`, `--reference-end-guard`             | `<INT>`        | Reference end guard.                                                                                                     | `0`                 |
| `-tb`, `--target-begin-guard`              | `<INT>`        | Target begin guard.                                                                                                      | `0`                 |
| `-te`, `--target-end-guard`                | `<INT>`        | Target end guard.                                                                                                        | `0`                 |
| `-ar`, `--asymmetric-regions`              | `-`            | Consider asymmetric regions.                                                                                             | Disabled            |
| `-nr`, `--no-self-complexity`              | `-`            | Skip self-complexity computation.                                                                                        | Disabled            |
| `-sb`, `--save-sequence`                   | `-`            | Keep temporary `.seq` files generated from FASTA/FASTQ input.                                                            | Disabled            |
| `-sp`, `--save-profile`                    | `-`            | Save profile output.                                                                                                     | Disabled            |
| `-sf`, `--save-filtered`                   | `-`            | Save filtered output.                                                                                                    | Disabled            |
| `-ss`, `--save-segmented`                  | `-`            | Save extracted segment files.                                                                                            | Disabled            |
| `-sa`, `--save-profile-filtered-segmented` | `-`            | Save profile, filtered, and segmented outputs.                                                                           | Disabled            |
| `-rm`, `--reference-model`                 | `<STRING>`     | Custom reference model chain.                                                                                            | Auto from `--level` |
| `-tm`, `--target-model`                    | `<STRING>`     | Custom target model chain.                                                                                               | Auto from `--level` |
| `-ll`, `--list-levels`                     | `-`            | Print the built-in compression levels.                                                                                   | -                   |
| `-h`, `--help`                             | `-`            | Show the help message.                                                                                                   | -                   |
| `-v`, `--verbose`                          | `-`            | Print detailed progress information.                                                                                     | Disabled            |
| `-V`, `--version`                          | `-`            | Show the program version.                                                                                                | -                   |

#### Model Parameter Fields

Custom model strings use the form `k,[w,d,]ir,a,g/t,ir,a,g:...`.

| Field | Meaning                                                                          |
| ----- | -------------------------------------------------------------------------------- |
| `k`   | Context size.                                                                    |
| `w`   | Sketch width given in log2 form, for example `10` means $2^{10} = 1024$.         |
| `d`   | Sketch depth.                                                                    |
| `ir`  | Inverted-repeat mode: `0` regular, `1` inverted only, `2` regular plus inverted. |
| `a`   | Estimator.                                                                       |
| `g`   | Forgetting factor in the range `0.0` to `1.0`.                                   |
| `t`   | Threshold for the number of substitutions in a tolerant model.                   |

### Output Compatibility

Smash++ output is deterministic for the same executable, options, input files, and platform. Profile files saved with `-sp` or `-sa` still serialize entropy values using the profile precision shown by the program, but filtering and segmentation use full-precision entropy internally.

Because of that, `.fil`, `.pos`, and `.json` output may differ slightly from older Smash++ releases in the final decimal places or in threshold-adjacent segment boundaries. These differences are deterministic and come from avoiding an older round-to-text-and-parse-back step in the compression hot path.

`--approx-sampled-models` is opt-in. It speeds up sampled multi-model runs by updating only contexts between sampled positions, so its `.prf`, `.fil`, `.pos`, and `.json` output should be treated as an approximate mode rather than byte-compatible output with the default model update path.

### Visualizer Options

Use `smashpp viz --help` to print the full CLI help.

| Option                             | Value        | Description                                                       | Default      |
| ---------------------------------- | ------------ | ----------------------------------------------------------------- | ------------ |
| `<POS_FILE>`                       | File         | Position file generated by Smash++ in `*.pos` or `*.json` format. | Required     |
| `-o`, `--output`                   | `<SVG_FILE>` | Output SVG path.                                                  | `map.svg`    |
| `-rn`, `--reference-name`          | `<STRING>`   | Override the displayed reference label.                           | Header value |
| `-tn`, `--target-name`             | `<STRING>`   | Override the displayed target label.                              | Header value |
| `-l`, `--link`                     | `<INT>`      | Link style between the two maps.                                  | `1`          |
| `-c`, `--color`                    | `<INT>`      | Color mode: `0` or `1`.                                           | `0`          |
| `-p`, `--opacity`                  | `<FLOAT>`    | Connector opacity.                                                | `0.9`        |
| `-w`, `--width`                    | `<INT>`      | Sequence bar width.                                               | `10`         |
| `-s`, `--space`                    | `<INT>`      | Space between sequences.                                          | `40`         |
| `-tc`, `--total-colors`            | `<INT>`      | Total number of colors to use.                                    | Auto         |
| `-rt`, `--reference-tick`          | `<INT>`      | Reference tick spacing.                                           | Auto         |
| `-tt`, `--target-tick`             | `<INT>`      | Target tick spacing.                                              | Auto         |
| `-th`, `--tick-human-readable`     | `<INT>`      | Human-readable tick labels: `0` false, `1` true.                  | `1`          |
| `-m`, `--min-block-size`           | `<INT>`      | Minimum block size to display.                                    | `1`          |
| `-vv`, `--vertical-view`           | `-`          | Render a vertical layout.                                         | Disabled     |
| `-nrr`, `--no-relative-redundancy` | `-`          | Hide relative redundancy coloring.                                | Disabled     |
| `-nr`, `--no-redundancy`           | `-`          | Hide redundancy coloring.                                         | Disabled     |
| `-ni`, `--no-inverted`             | `-`          | Hide inverted matches.                                            | Disabled     |
| `-ng`, `--no-regular`              | `-`          | Hide regular matches.                                             | Disabled     |
| `-n`, `--show-N`                   | `-`          | Highlight `N` bases.                                              | Disabled     |
| `-stat`, `--statistics`            | `-`          | Save statistics to CSV.                                           | `stat.csv`   |
| `-h`, `--help`                     | `-`          | Show the help message.                                            | -            |
| `-v`, `--verbose`                  | `-`          | Print detailed plotting information.                              | Disabled     |
| `-V`, `--version`                  | `-`          | Show the program version.                                         | -            |

### Example

After running the default installer, the example workflow looks like this:

```sh
cd example
../dist/bin/smashpp -r ref -t tar
../dist/bin/smashpp viz -o example.svg ref.tar.pos
```

JSON output is available too:

```sh
cd example
../dist/bin/smashpp --reference ref --target tar --format json
../dist/bin/smashpp viz --output example.svg ref.tar.json
```

If `smashpp` is already on your `PATH`, you can drop the `../dist/bin/` prefix.

## Testing and Benchmarks

After configuring and building from source, run the regression suite with:

```sh
ctest --test-dir build --output-on-failure
```

To make warnings fail the build in local development or CI, configure with:

```sh
cmake -S . -B build -DSMASHPP_STRICT_WARNINGS=ON
```

For local performance checks, run the benchmark target:

```sh
cmake --build build --target smashpp-benchmark
```

To compare against another executable configure with:

```sh
cmake -S . -B build -DSMASHPP_BENCHMARK_BASELINE=/path/to/other/smashpp
cmake --build build --target smashpp-benchmark
```

The benchmark generates deterministic small and large inputs and writes timing rows to `build/benchmarks/summary.csv`. The default large benchmark input is 256 MiB per file. Override the generated input sizes with byte counts when you need a shorter smoke run or a larger production check:

```sh
cmake -S . -B build \
  -DSMASHPP_BENCHMARK_SMALL_BYTES=131072 \
  -DSMASHPP_BENCHMARK_LARGE_BYTES=268435456
```

Use the same compiler, build type, input sizes, and machine when comparing results.

To create portable release archives from the install rules, run:

```sh
cmake --build build --target package
```

The archives are written to `build/packages/`.

## Cite

If you find Smash++ useful in your research, please acknowledge our work by citing:

- M. Hosseini, D. Pratas, B. Morgenstern, A.J. Pinho, "Smash++: an alignment-free and memory-efficient tool to find genomic rearrangements," _GigaScience_, vol. 9, no. 5, 2020. [DOI: 10.1093/gigascience/giaa048](https://doi.org/10.1093/gigascience/giaa048)

## Issues

If you encounter an [issue](https://github.com/smortezah/smashpp/issues), please let us know.

## License

Smash++ is distributed under the [GNU GPL v3](http://www.gnu.org/licenses/gpl-3.0.html) license.
