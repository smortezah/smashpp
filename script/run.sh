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

  # ### Small (S) sizes: ref:1.000, tar:1.000
  # ./goose-fastqsimulation \
  #   -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 50 -n 5 -s 201   r_a
  # ./goose-fastqsimulation \
  #   -eh -eo -es -edb -rm 0 -f 0.20,0.30,0.30,0.20,0.0 -ls 50 -n 5 -s 58    r_b
  # ./goose-fastqsimulation \
  #   -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 50 -n 5 -s 3     r_c
  # ./goose-fastqsimulation \
  #   -eh -eo -es -edb -rm 0 -f 0.30,0.20,0.20,0.30,0.0 -ls 50 -n 5 -s 1797  r_d
  # cat r_a r_b r_c r_d > RefS

  # ./goose-mutatedna -mr 0.01 < r_b > t_a
  # ./smashpp-inv-rep            r_a   t_b
  # ./smashpp-inv-rep            r_d   t_c
  # cp                           r_c   t_d
  # cat t_a t_b t_c t_d > TarS

  # ### Medium (M) sizes: ref:100.000, tar:100.000
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

  # ### Large (L) sizes: ref:5.000.000, tar:5.000.000
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

  # ### Extra Large (XL) sizes: ref:100.000.000, tar:100.000.000
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
  # ./smashpp-inv-rep            r_c   t_b
  # cp                           r_d   t_c
  # ./smashpp-inv-rep            r_b   t_d
  # cat t_a t_b t_c t_d > TarXL

  ### Mutated (Mut) sizes: ref:10.000.000, tar:10.000.000. Up to 40% of mutation
  rm -f RefMut TarMut
  
  for i in {1..40}; do
    ./goose-fastqsimulation -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 \
      -ls 100 -n 2500 -s $i  r_$i
    cat r_$i >> RefMut

    ./goose-mutatedna -mr $(bc<<<"scale=2; $i/100") < r_$i > t_$i
    cat t_$i >> TarMut
  done

  rm r_* t_*
fi

### Get real dataset
#if [[ $DATASET_REAL -eq 1 ]]; then
#fi

### Run tests
if [[ $RUN -eq 1 ]]; then
  ### S
  ./smashpp -r RefS -t TarS -l 3 -w 45
  ./smashpp -viz -p 1 -b 3 -o S.svg RefS-TarS.pos
  ### M
  ./smashpp -r RefM -t TarM -l 3 -w 100
  ./smashpp -viz -p 1 -b 2 -f 80 -o M.svg RefM-TarM.pos
  ### L
  ./smashpp -r RefL -t TarL -l 3 -w 135
  ./smashpp -viz -p 1 -f 100 -o L.svg RefL-TarL.pos
  ### XL
  ./smashpp -r RefXL -t TarXL -l 3 -w 275
  ./smashpp -viz -p 1 -m 20000000 -o XL.svg RefXL-TarXL.pos
  ### Mut
  ./smashpp -r RefMut -t TarMut -th 1.5 -l 5 -w 5000
  ./smashpp -viz -p 1 -b 2 -f 50 -o Mut.svg RefMut-TarMut.pos
  ./smash -wt 1 -d 1 -w 5000 RefMut TarMut
fi