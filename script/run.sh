#!/usr/bin/env bash

GET_GOOSE=0;
SYNTHETIC=0;
REAL=0;

### Get GOOSE
if [[ $GET_GOOSE -eq 1 ]]; then
  rm -fr goose/ goose-*
  git clone https://github.com/pratas/goose.git
  cd goose/src/
  make -j7
  cp goose-fastqsimulation goose-mutatedna ../..
fi

### Simulate synthetic dataset
if [[ $SYNTHETIC -eq 1 ]]; then
  ### Small sizes: ref:1000, tar:900
  ./goose-fastqsimulation -eh -eo -es -edb -rn 2 -ri 10 -ra 50 -ls 100 -n 2 \
                          -f 0.2,0.3,0.3,0.2,0.0 -s 101                      r_a
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 10 -ra 50 -ls 100 -n 2 \
                          -f 0.25,0.25,0.25,0.25,0.0 -s 68                   r_b
  ./goose-fastqsimulation -eh -eo -es -edb -rn 3 -ri 10 -ra 50 -ls 100 -n 2 \
                          -f 0.25,0.25,0.25,0.25,0.0 -s 3                    r_c
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 10 -ra 50 -ls 100 -n 2 \
                          -f 0.3,0.2,0.2,0.3,0.0 -s 1785                     r_d
  ./goose-fastqsimulation -eh -eo -es -edb -rn 4 -ri 10 -ra 50 -ls 100 -n 2 \
                          -f 0.25,0.25,0.25,0.25,0.0 -s 228                  r_e
  cat r_a r_b r_c r_d r_e > refs

  ./goose-mutatedna -mr 0.05 < r_d > t_a
  ./goose-mutatedna -mr 0.03 < r_e > t_b
  ./goose-mutatedna -mr 0.10 < r_a > t_c
  ./goose-mutatedna -mr 0.02 < r_c > t_d
  ./goose-mutatedna -mr 0.01 < r_b > t_e
  cat t_a t_b t_c t_d t_e > t_f
  tail -c 900 t_f > tars

  ### Medium sizes: ref:100000, tar:95000
  ./goose-fastqsimulation -eh -eo -es -edb -rn 2 -ri 50 -ra 500 -ls 100 -n 200 \
                          -f 0.25,0.25,0.25,0.25,0.0 -s 1001                 r_a
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 50 -ra 500 -ls 100 -n 200 \
                          -f 0.3,0.2,0.2,0.3,0.0 -s 608                      r_b
  ./goose-fastqsimulation -eh -eo -es -edb -rn 3 -ri 50 -ra 500 -ls 100 -n 200 \
                          -f 0.2,0.3,0.3,0.2,0.0 -s 30                       r_c
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 50 -ra 500 -ls 100 -n 200 \
                          -f 0.25,0.25,0.25,0.25,0.0 -s 17805                r_d
  ./goose-fastqsimulation -eh -eo -es -edb -rn 4 -ri 50 -ra 500 -ls 100 -n 200 \
                          -f 0.25,0.25,0.25,0.25,0.0 -s 2028                 r_e
  cat r_a r_b r_c r_d r_e > refm

  ./goose-mutatedna -mr 0.05 < r_e > t_a
  ./goose-mutatedna -mr 0.03 < r_c > t_b
  ./goose-mutatedna -mr 0.10 < r_b > t_c
  ./goose-mutatedna -mr 0.02 < r_d > t_d
  ./goose-mutatedna -mr 0.01 < r_a > t_e
  cat t_a t_b t_c t_d t_e > t_f
  head -c 95000 t_f > tarm

  ### Large sizes: ref:5000000, tar:4999900
  ./goose-fastqsimulation -eh -eo -es -edb -rn 2 -ri 500 -ra 1500 \
                          -ls 100 -n 10000 -f 0.3,0.2,0.3,0.2,0.0 -s 10101   r_a
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 500 -ra 1500 \
                          -ls 100 -n 10000 -f 0.3,0.2,0.2,0.3,0.0 -s 6018    r_b
  ./goose-fastqsimulation -eh -eo -es -edb -rn 3 -ri 500 -ra 1500 \
                          -ls 100 -n 10000 -f 0.25,0.25,0.25,0.25,0.0 -s 10  r_c
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 500 -ra 1500 \
                          -ls 100 -n 10000 -f 0.25,0.25,0.25,0.25,0.0 -s 7   r_d
  ./goose-fastqsimulation -eh -eo -es -edb -rn 4 -ri 500 -ra 1500 \
                          -ls 100 -n 10000 -f 0.3,0.2,0.2,0.3,0.0 -s 20218   r_e
  cat r_a r_b r_c r_d r_e > refl

  ./goose-mutatedna -mr 0.05 < r_c > t_a
  ./goose-mutatedna -mr 0.03 < r_b > t_b
  ./goose-mutatedna -mr 0.10 < r_a > t_c
  ./goose-mutatedna -mr 0.02 < r_e > t_d
  ./goose-mutatedna -mr 0.01 < r_d > t_e
  cat t_a t_b t_c t_d t_e > t_f
  tail -c 4999900 t_f > tarl

  rm r_* t_*
fi

### Get real dataset
#if [[ $REAL -eq 1 ]]; then
#fi