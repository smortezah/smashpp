#!/usr/bin/env bash

GET_GOOSE=0;
DATASET_SYNTH=1;
DATASET_REAL=0;
RUN=0;

### Get GOOSE
if [[ $GET_GOOSE -eq 1 ]]; then
  rm -fr goose/ goose-*
  git clone https://github.com/pratas/goose.git
  cd goose/src/
  make -j7
  cp goose-fastqsimulation goose-mutatedna ../..
fi

### Simulate synthetic dataset
if [[ $DATASET_SYNTH -eq 1 ]]; then
  chmod 777 smashpp-inv-rep

  ### Small sizes: ref:1.000, tar:1.000
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 50 -n 5 -s 201   r_a
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.20,0.30,0.30,0.20,0.0 -ls 50 -n 5 -s 58    r_b
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 50 -n 5 -s 3     r_c
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.30,0.20,0.20,0.30,0.0 -ls 50 -n 5 -s 1797  r_d
  cat r_a r_b r_c r_d > RefS

  cp                           r_b   t_a
  ./smashpp-inv-rep            r_a   t_b
  ./smashpp-inv-rep            r_d   t_c
  cp                           r_c   t_d
  cat t_a t_b t_c t_d > TarS

  # ### Medium sizes: ref:100.000, tar:100.000
  # ./goose-fastqsimulation \
  #   -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 191  r_a
  # ./goose-fastqsimulation \
  #   -eh -eo -es -edb -rm 0 -f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250 -s 608  r_b
  # ./goose-fastqsimulation \
  #   -eh -eo -es -edb -rm 0 -f 0.20,0.30,0.30,0.20,0.0 -ls 100 -n 250 -s 30   r_c
  # ./goose-fastqsimulation \
  #   -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 138  r_d
  # cat r_a r_b r_c r_d > RefM

  # ./smashpp-inv-rep            r_b   t_a
  # cp                           r_c   t_b
  # ./goose-mutatedna -mr 0.01 < r_d > t_c
  # cp                           r_a   t_d
  # cat t_a t_b t_c t_d > TarM

  # ### Large sizes: ref:5.000.000, tar:5.000.000
  # ./goose-fastqsimulation -eh \
  #   -eo -es -edb -rm 0 -f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 12500 -s 10101  r_a
  # ./goose-fastqsimulation -eh \
  #   -eo -es -edb -rm 0 -f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 12500 -s 6018   r_b
  # ./goose-fastqsimulation -eh \
  #   -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 12500 -s 10     r_c
  # ./goose-fastqsimulation -eh \
  #   -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 12500 -s 7      r_d
  # cat r_a r_b r_c r_d > RefL
  
  # cp                           r_c   t_a
  # ./goose-mutatedna -mr 0.01 < r_b > t_b
  # cp                           r_a   t_c
  # ./smashpp-inv-rep            r_d   t_d
  # cat t_a t_b t_c t_d > TarL

  # ### Extra Large sizes: ref:100.000.000, tar:100.000.000
  # ./goose-fastqsimulation -eh \
  #   -eo -es -edb -rm 0 -f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 250000 -s 1311  r_a
  # ./goose-fastqsimulation -eh \
  #   -eo -es -edb -rm 0 -f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250000 -s 7129  r_b
  # ./goose-fastqsimulation -eh \
  #   -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 16    r_c
  # ./goose-fastqsimulation -eh \
  #   -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 537   r_d
  # cat r_a r_b r_c r_d > RefXL
  
  # ./goose-mutatedna -mr 0.01 < r_a > t_a
  # cp                           r_c   t_b
  # cp                           r_d   t_c
  # ./smashpp-inv-rep            r_b   t_d
  # cat t_a t_b t_c t_d > TarXL

  rm r_* t_*
fi

### Get real dataset
#if [[ $DATASET_REAL -eq 1 ]]; then
#fi

### Get real dataset
if [[ $RUN -eq 1 ]]; then
  ### S
  ### M
  ./smashpp -r RefM -t TarM -l 4 -w 100
  ./smashpp -viz -l 5 -p 1 -k 1000 -b 2 -m 80 -o M.svg RefM-TarM.pos
  ### L
  ./smashpp -r RefL -t TarL -l 4 -w 130
  ./smashpp -viz -l 5 -p 1 -m 100 -k 10000 -o L.svg RefL-TarL.pos
  ### XL
fi