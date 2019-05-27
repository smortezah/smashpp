import os
from shutil import copyfile
# import matplotlib.pyplot as plt
# import numpy as np

# Get dependencies
get_goose = False

# Make synthetic dataset
synth_small = False
synth_medium = False
synth_large = False
synth_xlarge = False
synth_mutation = False

# Run simulation
sim_small = False
sim_medium = False
sim_large = False
sim_xlarge = False
sim_mutation = False

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
current_dir = os.getcwd()
path_data = 'dataset' + sep + 'sim' + sep
path_bin = 'bin' + sep
goose_fastqsimulation = path_bin + 'goose-fastqsimulation '
goose_mutatedna = path_bin + 'goose-mutatedna '
smashpp = '..' + sep + '.' + sep + 'smashpp '
smashpp_inv_rep = path_bin + 'smashpp-inv-rep '
synth_common_par = '-eh -eo -es -edb -rm 0 '
sim_common_par = ' -w 15 -s 60 -vv '


def execute(cmd):
    os.popen(cmd).read()


def cat(file_in_names, file_out_name):
    with open(file_out_name, 'w') as file_out:
        for file in file_in_names:
            with open(file) as file_in:
                for line in file_in:
                    file_out.write(line)


def append(file_in_name, file_out_name):
    with open(file_out_name, 'a') as file_out:
        with open(file_in_name) as file_in:
            for line in file_in:
                file_out.write(line)


if get_goose:
    # Remove goose/, goose-*
    for file in os.listdir(current_dir):
        if file.startswith("goose-"):
            os.remove(os.path.join(current_dir, file))
    if os.path.exists("goose"):
        os.remove("goose")

    cmd = 'git clone https://github.com/pratas/goose.git;' + \
        'cd goose/src/;' + \
        'make -j7;' + \
        'cp goose-fastqsimulation goose-mutatedna ../..'
    os.popen(cmd).read()

# os.popen('sudo chmod 777 smashpp-inv-rep').read()
if synth_small:  # sizes: ref:1,000, tar:1,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 50 -n 5 -s 201  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.20,0.30,0.30,0.20,0.0 -ls 50 -n 5 -s 58  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 50 -n 5 -s 3  r_c')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.20,0.30,0.0 -ls 50 -n 5 -s 1797  r_d')
    cat(['r_a', 'r_b', 'r_c', 'r_d'], path_data + 'RefS')

    execute(goose_mutatedna + '-mr 0.01 < r_b > t_a')
    execute(smashpp_inv_rep + 'r_a t_b')
    execute(smashpp_inv_rep + 'r_d t_c')
    copyfile('r_c', 't_d')
    cat(['t_a', 't_b', 't_c', 't_d'], path_data + 'TarS')

if synth_medium:  # sizes: ref:100,000, tar:100,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 191  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250 -s 608  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.20,0.30,0.30,0.20,0.0 -ls 100 -n 250 -s 30  r_c')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 138  r_d')
    cat(['r_a', 'r_b', 'r_c', 'r_d'], path_data + 'RefM')

    execute(smashpp_inv_rep + 'r_b t_a')
    copyfile('r_c', 't_b')
    execute(goose_mutatedna + '-mr 0.01 < r_d > t_c')
    copyfile('r_a', 't_d')
    cat(['t_a', 't_b', 't_c', 't_d'], path_data + 'TarM')

if synth_large:  # sizes: ref:5,000,000, tar:5,000,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 12500 -s 10101  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 12500 -s 6018  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 12500 -s 10  r_c')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 12500 -s 7  r_d')
    cat(['r_a', 'r_b', 'r_c', 'r_d'], path_data + 'RefL')

    copyfile('r_c', 't_a')
    execute(goose_mutatedna + '-mr 0.01 < r_b > t_b')
    copyfile('r_a', 't_c')
    execute(smashpp_inv_rep + 'r_d t_d')
    cat(['t_a', 't_b', 't_c', 't_d'], path_data + 'TarL')

if synth_xlarge:  # sizes: ref:100,000,000, tar:100,000,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 250000 -s 1311  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250000 -s 7129  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 16  r_c')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 537  r_d')
    cat(['r_a', 'r_b', 'r_c', 'r_d'], path_data + 'RefXL')

    execute(goose_mutatedna + '-mr 0.01 < r_a > t_a')
    execute(smashpp_inv_rep + 'r_c t_b')
    copyfile('r_d', 't_c')
    execute(smashpp_inv_rep + 'r_b t_d')
    cat(['t_a', 't_b', 't_c', 't_d'], path_data + 'TarXL')

if synth_mutation:  # sizes:  ref:1,000,000, tar:1,000,000. Up to 60%
    if os.path.exists(path_data + "RefMut"):
        os.remove(path_data + "RefMut")
    if os.path.exists(path_data + "TarMut"):
        os.remove(path_data + "TarMut")

    for i in range(1, 60+1):
        execute(goose_fastqsimulation + synth_common_par + ' -s ' + str(i) +
                '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10 r_' + str(i))
        append('r_' + str(i), path_data + 'RefMut')

        execute(goose_mutatedna + '-mr ' + str(i/100) +
                ' < r_' + str(i) + ' > t_' + str(i))
        append('t_' + str(i), path_data + 'TarMut')

for file in os.listdir(current_dir):
    if file.startswith("r_"):
        os.remove(os.path.join(current_dir, file))
    if file.startswith("t_"):
        os.remove(os.path.join(current_dir, file))

if sim_small:
    ref = 'RefS'
    tar = 'TarS'
    out = 'S.svg'
    execute(smashpp + '-r ' + path_data +
            ref + ' -t ' + path_data + tar + ' -l 3 -f 45')
    execute(smashpp + '-viz -p 1 -b 200 -f 70 -o ' + out + sim_common_par +
            ref + '.' + tar + '.pos')

if sim_medium:
    ref = 'RefM'
    tar = 'TarM'
    out = 'M.svg'
    execute(smashpp + '-r ' + path_data +
            ref + ' -t ' + path_data + tar + ' -l 3 -f 100')
    execute(smashpp + '-viz -p 1 -b 50 -f 60 -o ' + out + sim_common_par +
            ref + '.' + tar + '.pos')

if sim_large:
    ref = 'RefL'
    tar = 'TarL'
    out = 'L.svg'
    execute(smashpp + '-r ' + path_data +
            ref + ' -t ' + path_data + tar + ' -l 3 -f 135')
    execute(smashpp + '-viz -p 1 -b 50 -f 60 -o ' + out + sim_common_par +
            ref + '.' + tar + '.pos')

if sim_xlarge:
    ref = 'RefXL'
    tar = 'TarXL'
    out = 'XL.svg'
    execute(smashpp + '-r ' + path_data +
            ref + ' -t ' + path_data + tar + ' -l 3 -f 275')
    execute(smashpp + '-viz -p 1 -m 20000000 -o ' + out + sim_common_par +
            ref + '.' + tar + '.pos')

if sim_mutation:
    ref = 'RefMut'
    tar = 'TarMut'
    out = 'Mut.svg'
    execute(smashpp + '-r ' + path_data + ref + ' -t ' +
            path_data + tar + ' -th 2 -rm 16,0,0.2,0.9 -f 25000 -m 15000')
    execute(smashpp + '-viz -p 1 -b 2 -f 50 -rt 5000 -tt 5000' +
            sim_common_par + '-o ' + out + ' ' + ref + '.' + tar + '.pos')
