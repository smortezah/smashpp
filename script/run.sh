#!/usr/bin/env bash

GET_GOOSE=0;
DATASET_SYNTH=0;
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

  ### Small (S) sizes: ref:1.000, tar:1.000
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 50 -n 5 -s 201   r_a
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.20,0.30,0.30,0.20,0.0 -ls 50 -n 5 -s 58    r_b
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 50 -n 5 -s 3     r_c
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.30,0.20,0.20,0.30,0.0 -ls 50 -n 5 -s 1797  r_d
  cat r_a r_b r_c r_d > RefS

  ./goose-mutatedna -mr 0.01 < r_b > t_a
  ./smashpp-inv-rep            r_a   t_b
  ./smashpp-inv-rep            r_d   t_c
  cp                           r_c   t_d
  cat t_a t_b t_c t_d > TarS

  ### Medium (M) sizes: ref:100.000, tar:100.000
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 191  r_a
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250 -s 608  r_b
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.20,0.30,0.30,0.20,0.0 -ls 100 -n 250 -s 30   r_c
  ./goose-fastqsimulation \
    -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 138  r_d
  cat r_a r_b r_c r_d > RefM

  ./smashpp-inv-rep            r_b   t_a
  cp                           r_c   t_b
  ./goose-mutatedna -mr 0.01 < r_d > t_c
  cp                           r_a   t_d
  cat t_a t_b t_c t_d > TarM

  ### Large (L) sizes: ref:5.000.000, tar:5.000.000
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 12500 -s 10101  r_a
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 12500 -s 6018   r_b
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 12500 -s 10     r_c
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 12500 -s 7      r_d
  cat r_a r_b r_c r_d > RefL
  
  cp                           r_c   t_a
  ./goose-mutatedna -mr 0.01 < r_b > t_b
  cp                           r_a   t_c
  ./smashpp-inv-rep            r_d   t_d
  cat t_a t_b t_c t_d > TarL

  ### Extra Large (XL) sizes: ref:100.000.000, tar:100.000.000
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 250000 -s 1311  r_a
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250000 -s 7129  r_b
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 16    r_c
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 537   r_d
  cat r_a r_b r_c r_d > RefXL
  
  ./goose-mutatedna -mr 0.01 < r_a > t_a
  ./smashpp-inv-rep            r_c   t_b
  cp                           r_d   t_c
  ./smashpp-inv-rep            r_b   t_d
  cat t_a t_b t_c t_d > TarXL

  ### Mutated (Mut) sizes: ref:10.000.000, tar:10.000.000
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 1196    r_a
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 31343   r_b
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 42927   r_c
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 85301   r_d
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 7290    r_e
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 35523   r_f
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 59236   r_g
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 57637   r_h
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 64620   r_i
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 58948   r_j
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 2638    r_k
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 53850   r_l
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 97564   r_m
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 75516   r_n
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 48232   r_o
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 8558    r_p
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 77964   r_q
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 89355   r_r
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 94679   r_s
  ./goose-fastqsimulation -eh \
    -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5000 -s 87501   r_t
  cat r_a r_b r_c r_d r_e r_f r_g r_h r_i r_j r_k r_l r_m r_n r_o r_p r_q r_r \   r_s r_t > RefMut

  ./goose-mutatedna -mr 0.01 < r_a > t_a
  ./goose-mutatedna -mr 0.02 < r_b > t_b
  ./goose-mutatedna -mr 0.03 < r_c > t_c
  ./goose-mutatedna -mr 0.04 < r_d > t_d
  ./goose-mutatedna -mr 0.05 < r_e > t_e
  ./goose-mutatedna -mr 0.06 < r_f > t_f
  ./goose-mutatedna -mr 0.07 < r_g > t_g
  ./goose-mutatedna -mr 0.08 < r_h > t_h
  ./goose-mutatedna -mr 0.09 < r_i > t_i
  ./goose-mutatedna -mr 0.10 < r_j > t_j
  ./goose-mutatedna -mr 0.11 < r_k > t_k
  ./goose-mutatedna -mr 0.12 < r_l > t_l
  ./goose-mutatedna -mr 0.13 < r_m > t_m
  ./goose-mutatedna -mr 0.14 < r_n > t_n
  ./goose-mutatedna -mr 0.15 < r_o > t_o
  ./goose-mutatedna -mr 0.16 < r_p > t_p
  ./goose-mutatedna -mr 0.17 < r_q > t_q
  ./goose-mutatedna -mr 0.18 < r_r > t_r
  ./goose-mutatedna -mr 0.19 < r_s > t_s
  ./goose-mutatedna -mr 0.20 < r_t > t_t
  cat t_a t_b t_c t_d t_e t_f t_g t_h t_i t_j t_k t_l t_m t_n t_o t_p t_q t_r \
    t_s t_t > TarMut

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
  ./smashpp -viz -p 1 -b 2 -t 80 -o M.svg RefM-TarM.pos
  ### L
  ./smashpp -r RefL -t TarL -l 3 -w 135
  ./smashpp -viz -p 1 -t 100 -o L.svg RefL-TarL.pos
  ### XL
  ./smashpp -r RefXL -t TarXL -l 3 -w 275
  ./smashpp -viz -p 1 -m 20000000 -o XL.svg RefXL-TarXL.pos
  ### Mut
  ./smashpp -r RefMut -t TarMut -th 1.5 -l 5 -w 5000
  ./smashpp -viz -p 1 -b 2 -t 50 -o Mut.svg RefMut-TarMut.pos
  ./smash -wt 1 -d 1 -w 5000 RefMut TarMut
fi