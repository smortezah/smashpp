#!/usr/bin/env bash

GET_GOOSE=0;
SIMULATE=1;

### GET GOOSE
if [[ $GET_GOOSE -eq 1 ]]; then
  rm -fr goose/ goose-*
  git clone https://github.com/pratas/goose.git
  cd goose/src/
  make -j7
  cp goose-fastqsimulation goose-mutatedna ../..
fi

if [[ $SIMULATE -eq 1 ]]; then
  # Size: ref:1000, tar=900
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
  
  rm r_* t_*
fi