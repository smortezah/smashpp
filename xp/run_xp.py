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

# Run on simulated dataset
sim_small = True
sim_medium = False
sim_large = False
sim_xlarge = False
sim_mutation = False

# Run on real dataset
e_coli_s_dysenteriae = False
gga24_mga26 = False

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
current_dir = os.getcwd()
path_data_sim = 'dataset' + sep + 'sim' + sep
path_data_real = 'dataset' + sep
path_data_permute = 'dataset' + sep + 'permute' + sep
path_bin = 'bin' + sep
goose_fastqsimulation = path_bin + 'goose-fastqsimulation '
goose_mutatedna = path_bin + 'goose-mutatedna '
goose_permuteseqbyblocks = path_bin + 'goose-permuteseqbyblocks'
smashpp = '..' + sep + 'smashpp '
smashpp_inv_rep = path_bin + 'smashpp-inv-rep '
synth_common_par = '-eh -eo -es -edb -rm 0 '
# sim_common_par = ' -w 10 -s 19 -vv '
sim_common_par = ' -vv '


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
if synth_small:  # sizes: ref:1,500, tar:1,500
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5 -s 201  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.20,0.30,0.30,0.20,0.0 -ls 100 -n 5 -s 58  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5 -s 15  r_c')
    cat(['r_a', 'r_b', 'r_c'], path_data_sim + 'RefS')

    execute(goose_mutatedna + '-mr 0.02 < r_a > t_b')
    execute(smashpp_inv_rep + 'r_b t_c')
    copyfile('r_c', 't_a')
    cat(['t_a', 't_b', 't_c'], path_data_sim + 'TarS')

if synth_medium:  # sizes: ref:100,000, tar:100,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 191  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250 -s 608  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.20,0.30,0.30,0.20,0.0 -ls 100 -n 250 -s 30  r_c')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 138  r_d')
    cat(['r_a', 'r_b', 'r_c', 'r_d'], path_data_sim + 'RefM')

    execute(smashpp_inv_rep + 'r_a t_d')
    execute(goose_mutatedna + '-mr 0.90 < r_b > t_c')
    copyfile('r_c', 't_a')
    execute(goose_mutatedna + '-mr 0.03 < r_d > t_b')
    cat(['t_a', 't_b', 't_c', 't_d'], path_data_sim + 'TarM')

if synth_large:  # sizes: ref:5,000,000, tar:5,000,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 25000 -s 10101  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 25000 -s 10  r_b')
    cat(['r_a', 'r_b'], path_data_sim + 'RefL')

    execute(smashpp_inv_rep + 'r_a t_b')
    execute(goose_mutatedna + '-mr 0.02 < r_b > t_a')
    cat(['t_a', 't_b'], path_data_sim + 'TarL')

if synth_xlarge:  # sizes: ref:100,000,000, tar:100,000,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 250000 -s 1311  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250000 -s 7129  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 16  r_c')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 537  r_d')
    cat(['r_a', 'r_b', 'r_c', 'r_d'], path_data_sim + 'RefXL')

    execute(goose_mutatedna + '-mr 0.01 < r_a > t_a')
    execute(smashpp_inv_rep + 'r_c t_b')
    copyfile('r_d', 't_c')
    execute(smashpp_inv_rep + 'r_b t_d')
    cat(['t_a', 't_b', 't_c', 't_d'], path_data_sim + 'TarXL')

if synth_mutation:  # sizes:  ref:1,000,000, tar:1,000,000. Up to 60%
    if os.path.exists(path_data_sim + "RefMut"):
        os.remove(path_data_sim + "RefMut")
    if os.path.exists(path_data_sim + "TarMut"):
        os.remove(path_data_sim + "TarMut")

    for i in range(1, 60+1):
        execute(goose_fastqsimulation + synth_common_par + ' -s ' + str(i) +
                '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10 r_' + str(i))
        append('r_' + str(i), path_data_sim + 'RefMut')

        execute(goose_mutatedna + '-mr ' + str(i/100) +
                ' < r_' + str(i) + ' > t_' + str(i))
        append('t_' + str(i), path_data_sim + 'TarMut')

for file in os.listdir(current_dir):
    if file.startswith("r_"):
        os.remove(os.path.join(current_dir, file))
    if file.startswith("t_"):
        os.remove(os.path.join(current_dir, file))

if sim_small:
    ref = 'RefS'
    tar = 'TarS'
    out = 'S.svg'
#     execute(smashpp + '-r ' + path_data_sim + ref +
#             ' -t ' + path_data_sim + tar + ' -l 3 -f 25')
#     execute(smashpp + '-viz -p 1 -b 5 -f 55 -rt 150 -tt 150 -th 0 ' +
#     execute(smashpp + '-viz -p 1 -rt 150 -tt 150 -th 0 ' +
    execute(smashpp + '-viz -p 1 -rt 150 -tt 150 ' +
            '-o ' + out + sim_common_par + ref + '.' + tar + '.pos')

if sim_medium:
    ref = 'RefM'
    tar = 'TarM'
    out = 'M.svg'
    execute(smashpp + '-r ' + path_data_sim +
            ref + ' -t ' + path_data_sim + tar + ' -l 3 -f 100')
#     execute(smashpp + '-viz -p 1 -b 5 -f 55 -o ' + out +
    execute(smashpp + '-viz -p 1 -o ' + out +
            sim_common_par + ref + '.' + tar + '.pos')

if sim_large:
    ref = 'RefL'
    tar = 'TarL'
    out = 'L.svg'
    execute(smashpp + '-r ' + path_data_sim +
            ref + ' -t ' + path_data_sim + tar + ' -l 3 -f 135')
    execute(smashpp + '-viz -p 1 -b 5 -f 55 -o ' + out + sim_common_par +
            ref + '.' + tar + '.pos')

if sim_xlarge:
    ref = 'RefXL'
    tar = 'TarXL'
    out = 'XL.svg'
    execute(smashpp + '-r ' + path_data_sim +
            ref + ' -t ' + path_data_sim + tar + ' -l 3 -f 275')
    execute(smashpp + '-viz -p 1 -m 20000000 -o ' + out + sim_common_par +
            ref + '.' + tar + '.pos')

if sim_mutation:
    ref = 'RefMut'
    tar = 'TarMut'
    out = 'Mut.svg'
    execute(smashpp + '-r ' + path_data_sim + ref + ' -t ' +
            path_data_sim + tar + ' -th 2 -rm 16,0,0.2,0.9 -f 25000 -m 15000')
    execute(smashpp + '-viz -p 1 -b 2 -f 50 -rt 5000 -tt 5000' +
            sim_common_par + '-o ' + out + ' ' + ref + '.' + tar + '.pos')

if e_coli_s_dysenteriae:
    path = path_data_real + 'bacteria' + sep
    ref = 'e_coli_O104_H4.seq'
    tar = 's_dysenteriae_chr.seq'
    out = 'e_coli_s_dysenteriae.svg'
    execute(smashpp + '-r ' + path + ref + ' -t ' + path + tar +
            ' -th 1.8 -l 3 -f 275 -m 7500')
    # execute(smashpp + '-viz -p 1 -w 15 -s 60 -vv -o ' + out + ' ' +
    #         ref + '.' + tar + '.pos')

if gga24_mga26:
    main = False
    permute_1M = False
    permute_500K = False

    tar = '26'
    path_tar = path_data_real + 'bird' + sep + \
        'Meleagris_gallopavo' + sep + tar  # + '.seq'

    if main:
        ref = '24'
        out = 'gga24_mga26.svg'
        path_ref = path_data_real + 'bird' + sep + \
            'Gallus_gallus' + sep + ref  # + '.seq'
        execute(smashpp + '-r ' + path_ref + ' -t ' + path_tar +
                ' -rm 20,0,0.001,0.9 -m 150000 -th 1.9 -d 6000 -f 175 -sf -nr -ar')
                # ' -rm 20,0,0.001,0.9 -m 150000 -th 1.84 -d 4000 -f 270 -ar')
        execute(smashpp + '-viz -p 1 -l 6 ' + sim_common_par +
                '-o ' + out + ' ' + ref + '.' + tar + '.pos')

    if permute_1M:
        ref = 'gga24_1M'
        out = 'gga24_mga26_perm1M.svg'
        main_in = '24'
        path_in = path_data_real + 'bird' + sep + \
            'Gallus_gallus' + sep + main_in  # + '.seq'
        path_ref = path_data_permute + ref  # + '.seq'
        # execute(goose_permuteseqbyblocks + ' -bs 1000000 -s 7 < ' +
        #         path_in + ' > ' + path_ref)
        execute(smashpp + '-r ' + path_ref + ' -t ' + path_tar +
                ' -rm 20,0,0.001,0.9 -m 150000 -th 1.9 -d 6000 -f 175 -ar -sf -nr')
                # ' -rm 20,0,0.001,0.9 -m 150000 -th 1.84 -d 4000 -f 270 -ar -sf')
        execute(smashpp + '-viz -p 1 -l 6 ' + sim_common_par +
                '-o ' + out + ' ' + ref + '.' + tar + '.pos')

    if permute_500K:
        ref = 'gga24_500K'
        out = 'gga24_mga26_perm500K.svg'
        main_in = '24'
        path_in = path_data_real + 'bird' + sep + \
            'Gallus_gallus' + sep + main_in  # + '.seq'
        path_ref = path_data_permute + ref  # + '.seq'
        execute(goose_permuteseqbyblocks + ' -bs 500000 -s 11 < ' +
                path_in + ' > ' + path_ref)
        execute(smashpp + '-r ' + path_ref + ' -t ' + path_tar +
                ' -rm 20,0,0.001,0.9 -m 150000 -th 1.9 -d 6000 -f 175 -ar -sf -nr')
        execute(smashpp + '-viz -p 1 -l 6 ' + sim_common_par +
                '-o ' + out + ' ' + ref + '.' + tar + '.pos')
