#!/usr/bin/env bash

GET_GOOSE=0;
SYNTHETIC=1;
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
  ### Small sizes: ref:1000, tar:1000
  ./goose-fastqsimulation -eh -eo -es -edb -rn 2 -ri 5 -ra 15 -rm 0 -s 101 \
                          -ls 50 -n 5 -f 0.2,0.3,0.3,0.2,0.0                 r_a
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 5 -ra 20 -rm 0 -s 68 \
                          -ls 50 -n 5 -f 0.25,0.25,0.25,0.25,0.0             r_b
  ./goose-fastqsimulation -eh -eo -es -edb -rn 3 -ri 5 -ra 10 -rm 0 -s 3 \
                          -ls 50 -n 5 -f 0.25,0.25,0.25,0.25,0.0             r_c
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 5 -ra 20 -rm 0 -s 1785 \
                          -ls 50 -n 5 -f 0.3,0.2,0.2,0.3,0.0                 r_d
  cat r_a r_b r_c r_d > refs

  ./goose-mutatedna -mr 0.03 < r_d > t_a
  ./goose-mutatedna -mr 0.05 < r_b > t_c
  ./goose-mutatedna -mr 0.10 < r_a > t_b
  ./goose-mutatedna -mr 0.15 < r_c > t_d
  cat t_a t_b t_c t_d > tars

  ### Medium sizes: ref:100000, tar:100000
  ./goose-fastqsimulation -eh -eo -es -edb -rn 2 -ri 50 -ra 500 -rm 0 -s 1001 \
                          -ls 100 -n 250 -f 0.25,0.25,0.25,0.25,0.0          r_a
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 50 -ra 500 -rm 0 -s 608 \
                          -ls 100 -n 250 -f 0.3,0.2,0.2,0.3,0.0              r_b
  ./goose-fastqsimulation -eh -eo -es -edb -rn 3 -ri 50 -ra 500 -rm 0 -s 30 \
                          -ls 100 -n 250 -f 0.2,0.3,0.3,0.2,0.0              r_c
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 50 -ra 500 -rm 0 -s 17805 \
                          -ls 100 -n 250 -f 0.25,0.25,0.25,0.25,0.0          r_d
  cat r_a r_b r_c r_d > refm

  ./goose-mutatedna -mr 0.03 < r_d > t_a
  ./goose-mutatedna -mr 0.05 < r_c > t_b
  ./goose-mutatedna -mr 0.10 < r_b > t_c
  ./goose-mutatedna -mr 0.15 < r_a > t_d
  cat t_a t_b t_c t_d > tarm

  ### Large sizes: ref:5000000, tar:5000000
  ./goose-fastqsimulation -eh -eo -es -edb -rn 2 -ri 500 -ra 1500 -rm 0 \
                          -ls 100 -n 12500 -f 0.3,0.2,0.3,0.2,0.0 -s 10101   r_a
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 500 -ra 1500 -rm 0 \
                          -ls 100 -n 12500 -f 0.3,0.2,0.2,0.3,0.0 -s 6018    r_b
  ./goose-fastqsimulation -eh -eo -es -edb -rn 3 -ri 500 -ra 1500 -rm 0 \
                          -ls 100 -n 12500 -f 0.25,0.25,0.25,0.25,0.0 -s 10  r_c
  ./goose-fastqsimulation -eh -eo -es -edb -rn 1 -ri 500 -ra 1500 -rm 0 \
                          -ls 100 -n 12500 -f 0.25,0.25,0.25,0.25,0.0 -s 7   r_d
  cat r_a r_b r_c r_d > refl

  ./goose-mutatedna -mr 0.03 < r_c > t_a
  ./goose-mutatedna -mr 0.05 < r_b > t_b
  ./goose-mutatedna -mr 0.10 < r_a > t_c
  ./goose-mutatedna -mr 0.15 < r_d > t_d
  cat t_a t_b t_c t_d > tarl

  rm r_* t_*
fi

### Get real dataset
#if [[ $REAL -eq 1 ]]; then
#fi