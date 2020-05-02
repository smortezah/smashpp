# Smash++ &nbsp;&nbsp;&nbsp;&nbsp; [![Build Status](https://travis-ci.com/smortezah/smashpp.svg?token=EWxsPpL9t9UvE93uKjH5&branch=master)](https://travis-ci.com/smortezah/smashpp) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/028cee56c77046dea4bc870237aae46a)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=smortezah/smashpp&amp;utm_campaign=Badge_Grade) ![Conda](https://img.shields.io/conda/dn/bioconda/smashpp) [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)

A fast tool to find and visualize rearrangements in DNA sequences.

## Install
To install Smash++ on various operating systems, follow the instructions below. It requires CMake (>= 3.9) and a C++14 compliant compiler. Note that a precompiled executable is available for 64 bit operating systems in the `experiment/bin` directory.

### Conda
Install [Miniconda](https://docs.conda.io/en/latest/miniconda.html), then run the following:
```bash
conda install -y -c bioconda smashpp
```

### Linux
* Install Git and CMake:
```bash
  sudo apt update
  sudo apt install git cmake
```
* Clone Smash++ and install it:
```bash
  git clone https://github.com/smortezah/smashpp.git
  cd smashpp
  ./install.sh
```

### macOS
* Install Homebrew, Git and CMake:
```bash
  /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  brew install git cmake
```
* Clone Smash++ and install it:
```bash
  git clone https://github.com/smortezah/smashpp.git
  cd smashpp
  ./install.sh
```

### Windows
Install [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10) (Windows Subsystem for Linux), then clone Smash++ and install it, like in Linux/macOS:
```bat
git clone https://github.com/smortezah/smashpp.git
cd smashpp
./install.sh
```

## Run
```text
./smashpp [OPTIONS]  -r <REF-FILE>  -t <TAR-FILE>
```

For example,
```text
./smashpp -r ref -t tar
```

It is recommended to choose short names for reference and target 
sequences.

### Options
To see the possible options for Smash++, type:
```bash
./smashpp
```

which provides the following:
```text
SYNOPSIS
  ./smashpp [OPTIONS]  -r <REF-FILE>  -t <TAR-FILE>

OPTIONS
  Required:
  -r  <FILE>         = reference file (Seq/FASTA/FASTQ)
  -t  <FILE>         = target file    (Seq/FASTA/FASTQ)

  Optional:
  -l  <INT>          = level of compression: [0, 6]. Default -> 3
  -m  <INT>          = min segment size: [1, 4294967295]     -> 50
  -e  <FLOAT>        = entropy of 'N's: [0.0, 100.0]         -> 2.0
  -n  <INT>          = number of threads: [1, 255]           -> 4
  -f  <INT>          = filter size: [1, 4294967295]          -> 100
  -ft <INT/STRING>   = filter type (windowing function):     -> hann
                       {0/rectangular, 1/hamming, 2/hann,
                       3/blackman, 4/triangular, 5/welch,
                       6/sine, 7/nuttall}
  -fs [S][M][L]      = filter scale:
                       {S/small, M/medium, L/large}
  -d  <INT>          = sampling steps                        -> 1
  -th <FLOAT>        = threshold: [0.0, 20.0]                -> 1.5
  -rb <INT>          = ref beginning guard: [-32768, 32767]  -> 0
  -re <INT>          = ref ending guard: [-32768, 32767]     -> 0
  -tb <INT>          = tar beginning guard: [-32768, 32767]  -> 0
  -te <INT>          = tar ending guard: [-32768, 32767]     -> 0
  -ar                = consider asymmetric regions           -> no
  -nr                = do NOT compute self complexity        -> no
  -sb                = save sequence (input: FASTA/FASTQ)    -> no
  -sp                = save profile (*.prf)                  -> no
  -sf                = save filtered file (*.fil)            -> no
  -ss                = save segmented files (*.s[i])         -> no
  -sa                = save profile, filetered and           -> no
                       segmented files
  -rm k,[w,d,]ir,a,g/t,ir,a,g:...
  -tm k,[w,d,]ir,a,g/t,ir,a,g:...
                     = parameters of models
                <INT>  k:  context size
                <INT>  w:  width of sketch in log2 form,
                           e.g., set 10 for w=2^10=1024
                <INT>  d:  depth of sketch
                <INT>  ir: inverted repeat: {0, 1, 2}
                           0: regular (not inverted)
                           1: inverted, solely
                           2: both regular and inverted
              <FLOAT>  a:  estimator
              <FLOAT>  g:  forgetting factor: [0.0, 1.0)
                <INT>  t:  threshold (no. substitutions)
  -ll                = list of compression levels
  -h                 = usage guide
  -v                 = more information
  --version          = show version

AUTHOR
  Morteza Hosseini     seyedmorteza@ua.pt

SAMPLE
  ./smashpp -r ref -t tar -l 0 -m 1000
```

To see the options for Smash++ Visualizer, type:
```bash
./smashpp -viz
```

which provides the following:
```text
SYNOPSIS
  ./smashpp -viz [OPTIONS]  -o <SVG-FILE>  <POS-FILE>

OPTIONS
  Required:
  <POS-FILE>         = position file, generated by
                       Smash++ tool (*.pos)

  Optional:
  -o  <SVG-FILE>     = output image name (*.svg).    Default -> map.svg
  -rn <STRING>       = reference name shown on output. If it
                       has spaces, use double quotes, e.g.
                       "Seq label". Default: name in header
                       of position file
  -tn <STRING>       = target name shown on output
  -l  <INT>          = type of the link between maps: [1, 6] -> 1
  -c  <INT>          = color mode: [0, 1]                    -> 0
  -p  <FLOAT>        = opacity: [0.0, 1.0]                   -> 0.9
  -w  <INT>          = width of the sequence: [8, 100]       -> 10
  -s  <INT>          = space between sequences: [5, 200]     -> 40
  -tc <INT>          = total number of colors: [1, 255]
  -rt <INT>          = reference tick: [1, 4294967295]
  -tt <INT>          = target tick: [1, 4294967295]
  -th [0][1]         = tick human readable: 0=false, 1=true  -> 1
  -m  <INT>          = minimum block size: [1, 4294967295]   -> 1
  -vv                = vertical view                         -> no
  -nrr               = do NOT show relative redundancy       -> no
                       (relative complexity)
  -nr                = do NOT show redunadancy               -> no
  -ni                = do NOT show inverse maps              -> no
  -ng                = do NOT show regular maps              -> no
  -n                 = show 'N' bases                        -> no
  -stat              = save stats (*.csv)                    -> stat.csv
  -h                 = usage guide
  -v                 = more information
  --version          = show version

AUTHOR
  Morteza Hosseini     seyedmorteza@ua.pt

SAMPLE
  ./smashpp -viz -vv -o simil.svg ref.tar.pos
```

### Example
After installing Smash++, copy its executable file into `example` directory and go to that directory:
```bash
cp smashpp example/
cd example/
```
There is in this directory two 1000 base sequences, the reference sequence named `ref`, and the target sequence, named `tar`. Now, run Smash++ and the visualizer:
```bash
./smashpp -r ref -t tar
./smashpp -viz -o example.svg ref.tar.pos
```

## Experiment
To reproduce results in the paper, we have provided the Python script `xp.py` in the `experiment/` directory, that can run Smash++ on synthetic and real genomic data. By this script, you can automatically make/download the datasets, in case of synthetic/real data, run Smash++ on those data using predefined parameters, and benchmark the method.

To use `xp.py`, you need to switch **False** to **True** for a desired dataset, in the beginnig of the file. Then, it runs Smash++ on that (those) dataset(s) and saves in the `result/` directory the results including:
- a `*.pos` file, which contains the positions of similar regions, plus self- and relative-redundancy values. It also includes in the header the parameters used to run Smash++, and sizes of the reference and the target files
- a `*.svg` file with the similar regions visualized. This file is the output of Smash++ visualizer.
- the `bench.csv` file, that provides time and memory usage of Smash++. In case of comparing with Smash (the first version), this file will provide the time and memory usage of Smash method, too.
- in some cases, there would be a `*.csv` file, including the number of regular and inverted regions among the detected rearrangements. This file is generated when `-stat` flag is enabled for Smash++ visualizer.

Note that `xp.py` requires `conda` for downloading the real dataset using Entrez Direct (EDirect) utility. If EDirect is not already installed, the script will automatically install it by `conda`.

## Cite
Please cite the following, if you use Smash++:
* M. Hosseini, D. Pratas, B. Morgenstern, A.J. Pinho, "Smash++: an alignment-free and memory-efficient tool to find genomic rearrangements," *GigaScience*, vol. , no. , 2020

## Issues
Please let us know if there is any 
[issues](https://github.com/smortezah/smashpp/issues).

## License
Copyright © 2018-2020 Morteza Hosseini -- IEETA, University of Aveiro, Portugal.

Smash++ is licensed under GNU GPL v3. For more information, click 
[here](http://www.gnu.org/licenses/gpl-3.0.html).