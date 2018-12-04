# Smash++

[![Build Status](https://travis-ci.com/smortezah/smashpp.svg?token=EWxsPpL9t9UvE93uKjH5&branch=master)](https://travis-ci.com/smortezah/smashpp)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/9f9f395ded7146c8a30b1e36c3ab98de)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=smortezah/smashpp&amp;utm_campaign=Badge_Grade)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)

A tool to find and visualize rearrangements in DNA sequences.

## INSTALL
Get Smash++ and make the project, using:
```bash
git clone https://github.com/smortezah/smashpp.git
cd smashpp
cmake .
make -j4
```
Note, an already compiled version is available for 64 bit Linux OS in
the `bin/` directory.


## RUN
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
which provides the following:
```
NAME
  Smash++ v18.11 - rearrangements finder

SYNOPSIS
  ./smashpp [OPTIONS]...  -r [REF-FILE] -t [TAR-FILE]

SAMPLE

DESCRIPTION
  Mandatory arguments:
  -r,  --ref FILE            reference file (Seq/Fasta/Fastq)
  -t,  --tar FILE            target file    (Seq/Fasta/Fastq)

  Options:
  -v,  --verbose             more information
  -l,  --level INT           level of compression [0;4]     COMPRESS
  -n,  --nthr  INT           number of threads [1;8]
  -fs, --filter-scale S|M|L  scale of the filter {S|small,    FILTER
                             M|medium, L|large}
  -w,  --wsize INT           window size [1;100000]           FILTER
  -wt, --wtype [0;7]         type of windowing function       FILTER
                             {0|rectangular, 1|hamming, 2|hann,
                             3|blackman, 4|triangular, 5|welch,
                             6|sine, 7|nuttall}
  -d,  --step   INT          sampling steps                   FILTER
  -th, --thresh FLOAT        threshold [0.0;20.0]             FILTER
  -sp, --save-profile        save profile                       SAVE
  -sf, --save-filter         save filtered file                 SAVE
  -sb, --save-seq            save sequence (input: Fasta/Fastq) SAVE
  -ss, --save-segment        save segmented files               SAVE
  -sa, --save-all            save profile, filetered and        SAVE
                             segmented files
  -R,  --report              save results in file "report"    REPORT
  -h,  --help                usage guide 
  -rm, --ref-model [𝜅,[𝑤,𝑑,]ir,𝛼,𝛾/𝜏,ir,𝛼,𝛾:...]                MODEL
  -tm, --tar-model [𝜅,[𝑤,𝑑,]ir,𝛼,𝛾/𝜏,ir,𝛼,𝛾:...]                MODEL
                             parameters of models
                       (INT) 𝜅:  context size 
                       (INT) 𝑤:  width of sketch in log2 form, 
                                 e.g., set 10 for w=2^10=1024
                       (INT) 𝑑:  depth of sketch
                             ir: inverted repeat {0, 1, 2}
                                 0: regular (not inverted)
                                 1: inverted, solely
                                 2: both regular and inverted
                     (FLOAT) 𝛼:  estimator
                     (FLOAT) 𝛾:  forgetting factor [0.0;1.0)
                       (INT) 𝜏:  threshold (no. substitutions)
```

To see the options for Smash++ Visualizer, type:
```bash
./smashpp -viz
```
which provides the following:
```
NAME
  Smash++ Visualizer v18.11 - Visualization of Samsh++ output

SYNOPSIS
  ./smashpp -viz [OPTION]...  -o [SVG-FILE] POS-FILE]

SAMPLE

DESCRIPTION
  Mandatory arguments:
  POS-FILE                   positions file, enerated by      INPUT
                             Smash++ tool (*.pos)

  Options:
  -v,  --verbose             more information
  -o,  --out SVG-FILE        output image name               OUTPUT
  -sp, --show-pos            show positions                    SHOW
  -sn, --show-nrc            show normalized relative          SHOW
                             compression (NRC)
  -sr, --show-redun          show self complexity              SHOW
  -ni, --dont-show-inv       do NOT show inverse maps       NO SHOW
  -nr, --dont-show-reg       do NOT show regular maps       NO SHOW
  -l,  --link  INT           type of the link between maps [1;5]
  -w,  --width INT           width of the image sequence [10;200]
  -s,  --space INT           space between sequences [0;200]
  -m,  --mult  INT           multiplication factor for
                             color ID [1;255]
  -b,  --begin INT           beginning of color ID [0;255]
  -c,  --min   INT           minimum block size to
                             consider [1;99999999]
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


## CITE
Please cite the following, if you use Smash++:
* In process ...


## RELEASES
* [Release](https://github.com/smortezah/smashpp/releases) 1: .


## ISSUES
Please let us know if there is any 
[issues](https://github.com/smortezah/smashpp/issues).


## LICENSE
Smash++ is under GPL v3 license. For more information, click 
[here](http://www.gnu.org/licenses/gpl-3.0.html).