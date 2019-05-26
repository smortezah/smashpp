import os
# import matplotlib.pyplot as plt
# import numpy as np

get_goose = False
make_dataset_synth = False
simulation = False

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
path_data = 'dataset' + sep + 'sim' + sep
smashpp_bin = '.' + sep + 'smashpp'


def execute(cmd):
    os.popen('..' + sep + cmd).read()


if get_goose:
    #   rm -fr goose/ goose-*
    #   git clone https://github.com/pratas/goose.git
    #   cd goose/src/
    #   make -j7
    #   cp goose-fastqsimulation goose-mutatedna ../..

if make_dataset_synth:
    #   sudo chmod 777 smashpp-inv-rep

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

  # Mutated (Mut) sizes: ref:10.000.000, tar:10.000.000. Up to 40% of mutation
    #   rm -f RefMut TarMut

    #   for i in {1..60}; do
    #     ./goose-fastqsimulation -eh -eo -es -edb -rm 0 -f 0.25,0.25,0.25,0.25,0.0 \
    #       -ls 100 -n 100 -s $i  r_$i
    #     cat r_$i >> RefMut

    #     ./goose-mutatedna -mr $(bc<<<"scale=2; $i/100") < r_$i > t_$i
    #     cat t_$i >> TarMut
    #   done

    #   rm r_* t_*

if simulation:
    gen_par = ' -w 15 -s 60 -vv '
    small = False
    medium = False
    large = False
    extra_large = False
    mutation = True

    if small:
        ref = 'RefS'
        tar = 'TarS'
        out = 'S.svg'
        execute(smashpp_bin + ' -r ' + path_data +
                ref + ' -t ' + path_data + tar + ' -l 3 -f 45')
        execute(smashpp_bin + ' -viz ' +
                '-p 1 -b 200 -f 70 -o ' + out + gen_par + ref + '.' + tar + '.pos')

    if medium:
        ref = 'RefM'
        tar = 'TarM'
        out = 'M.svg'
        execute(smashpp_bin + ' -r ' + path_data +
                ref + ' -t ' + path_data + tar + ' -l 3 -f 100')
        execute(smashpp_bin +
                ' -viz -p 1 -b 50 -f 60 -o ' + out + gen_par + ref + '.' + tar + '.pos')

    if large:
        ref = 'RefL'
        tar = 'TarL'
        out = 'L.svg'
        execute(smashpp_bin + ' -r ' + path_data +
                ref + ' -t ' + path_data + tar + ' -l 3 -f 135')
        execute(smashpp_bin + ' -viz -p 1 -b 50 -f 60 -o ' +
                out + gen_par + ref + '.' + tar + '.pos')

    if extra_large:
        ref = 'RefXL'
        tar = 'TarXL'
        out = 'XL.svg'
        execute(smashpp_bin + ' -r ' + path_data +
                ref + ' -t ' + path_data + tar + ' -l 3 -f 275')
        execute(smashpp_bin + ' -viz -p 1 -m 20000000 -o ' +
                out + gen_par + ref + '.' + tar + '.pos')

    if mutation:
        ref = 'RefMut'
        tar = 'TarMut'
        out = 'Mut.svg'
        execute(smashpp_bin + ' -r ' + path_data +
                ref + ' -t ' + path_data + tar + ' -th 2 -l 5 -f 25000 -m 15000')
        execute(smashpp_bin + ' -viz -p 1 -b 2 -f 50 -o ' +
                out + gen_par + ref + '.' + tar + '.pos')
