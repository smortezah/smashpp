# Smash++

[![Build Status](https://travis-ci.com/smortezah/smashpp.svg?token=EWxsPpL9t9UvE93uKjH5&branch=master)](https://travis-ci.com/smortezah/smashpp)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/028cee56c77046dea4bc870237aae46a)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=smortezah/smashpp&amp;utm_campaign=Badge_Grade)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)

## What is Smash++?
A fast tool to find and visualize rearrangements in DNA sequences.

## Install
### Conda


### Linux
Install "git" and "cmake":
```bash
sudo apt update
sudo apt install git cmake
```

Clone Smash++ and make the project:
```bash
git clone https://github.com/smortezah/smashpp.git
cd smashpp
./compile.sh
```

### macOS
Install "Homebrew", "git" and "cmake":
```bash
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install git cmake
```

Clone Smash++ and make the project:
```bash
git clone https://github.com/smortezah/smashpp.git
cd smashpp
./compile.sh
```

Note that a precompiled version is available for 64 bit Linux and macOS in the `bin/` directory.

## Run
```bash
./smashpp [OPTION]...  -r [REF_FILE] -t [TAR_FILE]
```
For example,
```bash
./smashpp -r ref -t tar
```
It is highly recommended to choose short names for reference and target 
sequences.

### Options
To see the possible options for Smash++, type:
```bash
./smashpp
```
which provides the following: **UPDATE** <!-- todo -->
```text
NAME
  Smash++ v19.01 - rearrangements finder

SYNOPSIS
  ./smashpp  OPTIONS...  -r REF-FILE  -t TAR-FILE

SAMPLE

DESCRIPTION
  Mandatory arguments
  -r,  --ref FILE            reference file (Seq/Fasta/Fastq)
  -t,  --tar FILE            target file    (Seq/Fasta/Fastq)

  Options
  -v,  --verbose             more information
  -l,  --level INT           level of compression: [0, 5]
  -m,  --min   INT           min segment size: [1, 4294967295]
  -nr, --no-redun            do NOT compute self complexity
  -e,  --ent-n FLOAT         Entropy of 'N's: [0.0, 100.0]
  -n,  --nthr  INT           number of threads: [1, 8]
  -fs, --filter-scale S|M|L  scale of the filter:
                             {S|small, M|medium, L|large}
  -w,  --filt_size INT           window size: [1, 4294967295]
  -wt, --filt_type INT/STRING    type of windowing function:
                             {0|rectangular, 1|hamming, 2|hann,
                             3|blackman, 4|triangular, 5|welch,
                             6|sine, 7|nuttall}
  -d,  --step   INT          sampling steps
  -th, --thresh FLOAT        threshold: [0.0, 20.0]
  -sp, --save-profile        save profile (*.prf)
  -sf, --save-filter         save filtered file (*.fil)
  -sb, --save-seq            save sequence (input: Fasta/Fastq)
  -ss, --save-segment        save segmented files (*-s_i)
  -sa, --save-all            save profile, filetered and
                             segmented files
  -h,  --help                usage guide
  -rm, --ref-model  k,[w,d,]ir,a,g/t,ir,a,g:...
  -tm, --tar-model  k,[w,d,]ir,a,g/t,ir,a,g:...
                             parameters of models
                       (INT) k:  context size
                       (INT) w:  width of sketch in log2 form,
                                 e.g., set 10 for w=2^10=1024
                       (INT) d:  depth of sketch
                       (INT) ir: inverted repeat: {0, 1, 2}
                                 0: regular (not inverted)
                                 1: inverted, solely
                                 2: both regular and inverted
                     (FLOAT) a:  estimator
                     (FLOAT) g:  forgetting factor: [0.0, 1.0)
                       (INT) t:  threshold (no. substitutions)
```

To see the options for Smash++ Visualizer, type:
```bash
./smashpp -viz
```
which provides the following: **UPDATE** <!-- todo -->
```text
NAME
  Smash++ Visualizer v19.01 - Visualization of Samsh++ output

SYNOPSIS
  ./smashpp -viz  OPTION...  -o SVG-FILE  POS-FILE

SAMPLE

DESCRIPTION
  Mandatory arguments:
  POS-FILE                   positions file, generated by
                             Smash++ tool (*.pos)

  Options:
  -v,  --verbose             more information
  -o,  --out SVG-FILE        output image name (*.svg)
  -vv, --vertical            vertical view
  -nn, --no-nrc              do NOT show normalized
                             relative compression (NRC)
  -nr, --no-redun            do NOT show self complexity
  -ni, --no-inv              do NOT show inverse maps
  -ng, --no-reg              do NOT show regular maps
  -l,  --link     INT        type of the link between maps: [1, 6]
  -c,  --color    INT        color mode: [0, 2]
  -p,  --opacity  FLOAT      opacity: [0.0, 1.0]
  -w,  --width    INT        width of the sequence: [15, 100]
  -s,  --space    INT        space between sequences: [15, 200]
  -f,  --mult     INT        multiplication factor for
                             color ID: [1, 255]
  -b,  --begin    INT        beginning of color ID: [0, 255]
  -rt, --ref-tick INT        reference tick: [1, 4294967295]
  -tt, --tar-tick INT        target tick: [1, 4294967295]
  -m,  --min      INT        minimum block size: [1, 4294967295]
  -h,  --help                usage guide
```

### Example

### Compare Smash++ with other methods
In order for comparison, you might set the parameters in 
"run.sh" bash script, then run it:
```bash
./run.sh
```
With this script, you can download the datasets, install the dependencies, 
install the other tools, run all the tools, and finally, visualize the results.

## Cite
Please cite the following, if you use Smash++:
* In process ...

## Releases
* [Release](https://github.com/smortezah/smashpp/releases) 1: .

## Issues
Please let us know if there is any 
[issues](https://github.com/smortezah/smashpp/issues).

## License
Smash++ is under GPL v3 license. For more information, click 
[here](http://www.gnu.org/licenses/gpl-3.0.html).