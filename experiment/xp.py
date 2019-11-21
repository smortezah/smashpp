'''
Smash++
Morteza Hosseini, Diogo Pratas, Armando J. Pinho
{seyedmorteza,pratas,ap}@ua.pt
Copyright (C) 2018-2019, IEETA/DETI, University of Aveiro, Portugal
'''

'''
USER CAN CHANGE HERE BY SWITCHING False/True
'''
from pathlib import Path
import shutil
import csv
import os
import subprocess

# Resolve dependencies
GET_GOOSE = False

# Make synthetic dataset
MAKE_SYNTH_SMALL = False
MAKE_SYNTH_MEDIUM = False
MAKE_SYNTH_LARGE = False
MAKE_SYNTH_XLARGE = False
MAKE_SYNTH_MUTATE = False
MAKE_SYNTH_COMPARE_SMASH = False
MAKE_SYNTH_PERMUTE = False

# Run (Benchmark)
RUN_SYNTH_SMALL = False
RUN_SYNTH_MEDIUM = False
RUN_SYNTH_LARGE = False
RUN_SYNTH_XLARGE = False
RUN_SYNTH_MUTATE = False
RUN_REAL_GGA18_MGA20 = False
RUN_REAL_GGA14_MGA16 = False
RUN_REAL_HS12_PT12 = False
RUN_REAL_PXO99A_MAFF311018 = False
RUN_SYNTH_COMPARE_SMASH = False  # Compare to Smash on synthetic data
RUN_REAL_COMPARE_SMASH = False  # Compare to Smash on real data
RUN_SYNTH_PERM_ORIGINAL = False
RUN_SYNTH_PERM_450000 = False
RUN_SYNTH_PERM_30000 = False
RUN_SYNTH_PERM_1000 = False
RUN_SYNTH_PERM_30 = False


'''
General
'''
if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
current_dir = os.getcwd()
path_data = 'dataset' + sep
path_data_synth = path_data + 'synth' + sep
path_data_permute = path_data + 'permute' + sep
path_bin = 'bin' + sep
goose_fastqsimulation = path_bin + 'goose-fastqsimulation '
goose_mutatedna = path_bin + 'goose-mutatedna '
goose_permuteseqbyblocks = path_bin + 'goose-permuteseqbyblocks '
smashpp_exe = '..' + sep + 'smashpp '
smashpp_inv_rep = path_bin + 'smashpp-inv-rep '
smash = path_bin + 'smash '
synth_common_par = '-eh -eo -es -edb -rm 0 '
synth_small_ref_name = 'RefS'
synth_small_tar_name = 'TarS'
synth_medium_ref_name = 'RefM'
synth_medium_tar_name = 'TarM'
synth_large_ref_name = 'RefL'
synth_large_tar_name = 'TarL'
synth_xlarge_ref_name = 'RefXL'
synth_xlarge_tar_name = 'TarXL'
synth_mutate_ref_name = 'RefMut'
synth_mutate_tar_name = 'TarMut'
synth_comp_smash_ref_name = 'RefComp'
synth_comp_smash_tar_name = 'TarComp'
synth_perm_ref_name = 'RefPerm'
synth_perm_tar_name = 'TarPerm'
real_comp_smash_ref_name = 'VII.seq'
real_comp_smash_tar_name = 'VII.seq'
real_PXO99A_MAFF311018_ref_name = 'PXO99A.seq'
real_PXO99A_MAFF311018_tar_name = 'MAFF_311018.seq'
real_PXO99A_MAFF311018_path_ref = path_data + 'bacteria' + sep + \
    'Xanthomonas_oryzae_pv_oryzae' + sep
real_PXO99A_MAFF311018_path_tar = real_PXO99A_MAFF311018_path_ref
real_gga18_mga20_ref_name = '18.seq'
real_gga18_mga20_tar_name = '20.seq'
real_gga18_mga20_path_ref = path_data + 'bird' + sep + 'Gallus_gallus' + sep
real_gga18_mga20_path_tar = path_data + 'bird' + sep + \
    'Meleagris_gallopavo' + sep
real_gga14_mga16_ref_name = '14.seq'
real_gga14_mga16_tar_name = '16.seq'
real_gga14_mga16_path_ref = path_data + 'bird' + sep + 'Gallus_gallus' + sep
real_gga14_mga16_path_tar = path_data + 'bird' + sep + \
    'Meleagris_gallopavo' + sep
real_hs12_pt12_path_ref = path_data + 'mammalia' + sep + 'Homo_sapiens' + sep
real_hs12_pt12_path_tar = path_data + 'mammalia' + sep + \
    'Pan_troglodytes' + sep
real_hs12_pt12_ref_name = '12.seq'
real_hs12_pt12_tar_name = '12.seq'
time_exe = '/usr/bin/time -v --output='
log_main = 'log_main'
log_viz = 'log_viz'
log_smash = 'log_smash'


def execute(cmd):
    os.popen(cmd).read()


def cat(file_in_names, file_out_name):
    with open(file_out_name, 'w') as file_out:
        for file in file_in_names:
            with open(file) as file_in:
                for line in file_in:
                    file_out.write(line)


def append(file_in_name, file_out_name):
    with open(file_in_name) as file_in, open(file_out_name, 'a') as file_out:
        for line in file_in:
            file_out.write(line)


def remove_all_ext(directory, extension):
    for file_name in os.listdir(directory):
        if file_name.endswith(extension):
            os.remove(os.path.join(directory, file_name))


def remove_all_start(directory, word):
    for file_name in os.listdir(directory):
        if file_name.startswith(word):
            os.remove(os.path.join(directory, file_name))


def remove_path(path):
    if os.path.exists(path):
        os.remove(path)


def make_path(path):
    if not os.path.exists(path):
        os.makedirs(path)


def remove(directory, pattern):
    for p in Path(directory).glob(pattern):
        p.unlink()


def bare_name(name):
    return os.path.basename(name)


def file_size(file):
    return os.path.getsize(file)


def to_seconds(hms):
    vals = hms.split(':')
    if len(vals) == 2:
        h = 0
        m = int(vals[0])
        s = float(vals[1])
    elif len(vals) == 3:
        h = int(vals[0])
        m = int(vals[1])
        s = float(vals[2])
    return str(h * 3600 + m * 60 + s)


'''
Resolve dependencies
'''
if GET_GOOSE:
    remove_all_start(current_dir, "goose-")
    remove_path("goose")
    execute('git clone https://github.com/pratas/goose.git;' +
            'cd goose/src/;' +
            'make -j8;' +
            'cp goose-fastqsimulation goose-mutatedna ../../bin')


'''
Make synthetic dataset
'''
# execute('sudo chmod 777 smashpp-inv-rep')

if MAKE_SYNTH_SMALL:  # Sizes: ref:1,500, tar:1,500
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5 -s 201  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.20,0.30,0.30,0.20,0.0 -ls 100 -n 5 -s 58  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5 -s 15  r_c')
    cat(['r_a', 'r_b', 'r_c'], path_data_synth + 'RefS')

    execute(smashpp_inv_rep + 'r_a t_c')
    execute(goose_mutatedna + '-mr 0.02 < r_b > t_b')
    execute(smashpp_inv_rep + 'r_c t_a')
    cat(['t_a', 't_b', 't_c'], path_data_synth + 'TarS')

if MAKE_SYNTH_MEDIUM:  # Sizes: ref:100,000, tar:100,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 191  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250 -s 608  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.20,0.30,0.30,0.20,0.0 -ls 100 -n 250 -s 30  r_c')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 138  r_d')
    cat(['r_a', 'r_b', 'r_c', 'r_d'], path_data_synth + 'RefM')

    execute(smashpp_inv_rep + 'r_a t_d')
    execute(goose_mutatedna + '-mr 0.90 < r_b > t_c')
    shutil.copyfile('r_c', 't_a')
    execute(goose_mutatedna + '-mr 0.03 < r_d > t_b')
    cat(['t_a', 't_b', 't_c', 't_d'], path_data_synth + 'TarM')

if MAKE_SYNTH_LARGE:  # Sizes: ref:5,000,000, tar:5,000,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 25000 -s 10101  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 25000 -s 10  r_b')
    cat(['r_a', 'r_b'], path_data_synth + 'RefL')

    execute(smashpp_inv_rep + 'r_a t_b')
    execute(goose_mutatedna + '-mr 0.02 < r_b > t_a')
    cat(['t_a', 't_b'], path_data_synth + 'TarL')

if MAKE_SYNTH_XLARGE:  # Sizes: ref:100,000,000, tar:100,000,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 250000 -s 1311  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250000 -s 7129  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 16  r_c')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 537  r_d')
    cat(['r_a', 'r_b', 'r_c', 'r_d'], path_data_synth + 'RefXL')

    execute(goose_mutatedna + '-mr 0.01 < r_a > t_a')
    execute(smashpp_inv_rep + 'r_c t_b')
    shutil.copyfile('r_d', 't_c')
    execute(smashpp_inv_rep + 'r_b t_d')
    cat(['t_a', 't_b', 't_c', 't_d'], path_data_synth + 'TarXL')

if MAKE_SYNTH_MUTATE:  # Sizes: ref:60,000, tar:60,000. Mutation up to 60%
    if os.path.exists(path_data_synth + "RefMut"):
        os.remove(path_data_synth + "RefMut")
    if os.path.exists(path_data_synth + "TarMut"):
        os.remove(path_data_synth + "TarMut")

    for i in range(1, 60+1):
        execute(goose_fastqsimulation + synth_common_par + ' -s ' + str(i) +
                '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10 r_' + str(i))
        append('r_' + str(i), path_data_synth + 'RefMut')

        execute(goose_mutatedna + '-mr ' + str(i/100) +
                ' < r_' + str(i) + ' > t_' + str(i))
        append('t_' + str(i), path_data_synth + 'TarMut')

if MAKE_SYNTH_COMPARE_SMASH:  # Sizes: ref:1,000,000, tar:1,000,000
    if os.path.exists(path_data_synth + "RefComp"):
        os.remove(path_data_synth + "RefComp")
    if os.path.exists(path_data_synth + "TarComp"):
        os.remove(path_data_synth + "TarComp")

    for i in range(0, 9 + 1):
        execute(goose_fastqsimulation + synth_common_par +
                ' -s ' + str(i * 10 + 1) +
                '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 1000 r_' + str(i))
        append('r_' + str(i), path_data_synth + 'RefComp')
    shutil.copyfile('r_0', 't_0')
    for i in range(1, 9 + 1):
        execute(goose_mutatedna + '-mr ' + str(i/100) +
                ' < r_' + str(i) + ' > t_' + str(i))
    for i in range(4, 6 + 1):
        execute(smashpp_inv_rep + 't_' + str(i) + ' t_' + str(i) + 'i')

    for i in range(0, 3 + 1):
        append('t_' + str(i), path_data_synth + 'TarComp')
    for i in range(4, 6 + 1):
        append('t_' + str(i) + 'i', path_data_synth + 'TarComp')
    for i in range(7, 9 + 1):
        append('t_' + str(i), path_data_synth + 'TarComp')

if MAKE_SYNTH_PERMUTE:  # Sizes: ref:3,000,000, tar:3,000,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 198  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 110  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 30091  r_c')
    cat(['r_a', 'r_b', 'r_c'], path_data_synth + 'RefPerm')

    execute(goose_mutatedna + '-mr 0.01 < r_a > t_c')
    execute(smashpp_inv_rep + 'r_b t_b')
    execute(goose_mutatedna + '-mr 0.02 < r_c > t_a')
    cat(['t_a', 't_b', 't_c'], path_data_synth + 'TarPerm')

remove_all_start(current_dir, "r_")
remove_all_start(current_dir, "t_")


'''
Run
'''
# def par_smashpp(key, arg):
#     key = str.lower(key)
#     if key=='synth_small':
#         if arg==

#     exe_main = {
#         'SYNTH_SMALL': '-d 1 -f 100',
#         'SYNTH_MEDIUM': '-d 100 -f 50',
#         'SYNTH_LARGE': '-d 100 -f 135',
#         'SYNTH_XLARGE': '-d 100 -f 275',
#         'SYNTH_MUTATE': '-th 1.97 -d 600 -f 100 -m 15000',
#         'REAL_GGA18_MGA20': '-rm 14,0,0.005,0.95/5,0,1,0.95 -f 130 ' +
#                             '-m 500000 -d 2200 -th 1.9',
#         'REAL_GGA14_MGA16': '-rm 14,0,0.005,0.95/5,0,0.99,0.95 -f 200 ' +
#                             '-d 1500 -th 1.95 -e 1.95 -m 400000',
#         'REAL_HS12_PT12': '-rm 14,0,0.001,0.95 -f 9000 -d 500 -th 1.9 ' +
#                           '-m 100000',
#         'REAL_PXO99A_MAFF311018': '-rm 13,0,0.005,1 -f 150 -m 10000 ' +
#                                   '-d 1000 -th 1.55 -ar',
#         'SYNTH_COMPARE_SMASH': '-th 1.7 -f 1000 -d 10 -m 1 -sf',
#         'REAL_COMPARE_SMASH': '-th 1.85 -f 370 -d 100 -ar -sf',
#         'SYNTH_PERM_ORIGINAL': '-l 0 -f 10 -d 3000',
#         'SYNTH_PERM_450000': '-l 0 -f 25 -d 3000 -ar',
#         'SYNTH_PERM_30000': '-l 0 -f 75 -d 1500 -ar',
#         'SYNTH_PERM_1000': '-l 0 -f 25 -d 300 -ar',
#         'SYNTH_PERM_30': '-l 0 -f 250 -d 1 -ar'
#     }

#     return switcher.get(str.upper(name), '-d 1 -f 100')


bench_result = []  # Name, Category, Size, Time, Memory
bench = False


def extract(file, key):
    import re
    with open(file) as f:
        for line in f:
            if re.search(key, line):
                split = re.split(' ', line)
                return split[-1].strip()


def calc_mem(log_main, log_viz=''):
    key = 'Maximum resident'
    mem_main = extract(log_main, key)
    if not log_viz:
        return int(mem_main)
    else:
        mem_viz = extract(log_viz, key)
        return max(int(mem_main), int(mem_viz))


def calc_elapsed(log_main, log_viz=''):
    key = 'Elapsed'
    elapsed_main = to_seconds(extract(log_main, key))
    if not log_viz:
        return f"{float(elapsed_main):.2f}"
    else:
        elapsed_viz = to_seconds(extract(log_viz, key))
        return f"{float(elapsed_main) + float(elapsed_viz):.2f}"


def calc_user_time(log_main, log_viz=''):
    key = 'User'
    user_time_main = extract(log_main, key)
    if not log_viz:
        return f"{float(user_time_main):.2f}"
    else:
        user_time_viz = extract(log_viz, key)
        return f"{float(user_time_main) + float(user_time_viz):.2f}"


def calc_system_time(log_main, log_viz=''):
    key = 'System'
    system_time_main = extract(log_main, key)
    if not log_viz:
        return f"{float(system_time_main):.2f}"
    else:
        system_time_viz = extract(log_viz, key)
        return f"{float(system_time_main) + float(system_time_viz):.2f}"


if RUN_SYNTH_SMALL:
    par_main = ' -l 3 -d 1 -f 100'
    par_viz = '-p 1 -rt 150 -tt 150 -l 1 -w 13 -vv -stat -o S.svg'
    ref = path_data_synth + synth_small_ref_name
    tar = path_data_synth + synth_small_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    # Run
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'Small'
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_SYNTH_MEDIUM:
    par_main = '-l 3 -d 100 -f 50'
    par_viz = '-p 1 -l 1 -w 13 -vv -stat -o M.svg'
    ref = path_data_synth + synth_medium_ref_name
    tar = path_data_synth + synth_medium_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    # Run
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'Medium'
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_SYNTH_LARGE:
    par_main = '-l 3 -d 100 -f 135'
    par_viz = '-p 1 -l 1 -w 13 -vv -stat -o L.svg'
    ref = path_data_synth + synth_large_ref_name
    tar = path_data_synth + synth_large_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    # Run
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'Large'
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_SYNTH_XLARGE:
    par_main = '-l 3 -d 100 -f 275'
    par_viz = '-p 1 -l 1 -w 13 -vv -stat -o XL.svg'
    ref = path_data_synth + synth_xlarge_ref_name
    tar = path_data_synth + synth_xlarge_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    # Run
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'XLarge'
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_SYNTH_MUTATE:
    par_main = '-th 1.97 -l 3 -d 600 -f 100 -m 15000'
    par_viz = '-p 1 -l 1 -w 13 -rt 5000 -tt 5000 -vv -stat -o Mut.svg'
    ref = path_data_synth + synth_mutate_ref_name
    tar = path_data_synth + synth_mutate_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    # Run
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'Mutate'
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_REAL_GGA18_MGA20:
    par_main = '-rm 14,0,0.005,0.95/5,0,1,0.95 -f 130 -m 500000 -d 2200 ' + \
        '-th 1.9'
    par_viz = '-l 1 -p 1 -vv -tc 6 -rn "GGA 18" -tn "MGA 20" ' + \
        '-stat -o GGA18_MGA20.svg'
    ref = real_gga18_mga20_path_ref + real_gga18_mga20_ref_name
    tar = real_gga18_mga20_path_tar + real_gga18_mga20_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    # Run
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'GGA18_MGA20'
    cat = 'Real'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_REAL_GGA14_MGA16:
    par_main = '-rm 14,0,0.005,0.95/5,0,0.99,0.95 -f 200 -d 1500 -th 1.95 ' + \
        '-e 1.95 -m 400000'
    par_viz = '-l 1 -vv -p 1 -rn "GGA 14" -tn "MGA 16" ' + \
        '-rt 1500000 -tt 1500000 -stat -o GGA14_MGA16.svg'
    ref = real_gga14_mga16_path_ref + real_gga14_mga16_ref_name
    tar = real_gga14_mga16_path_tar + real_gga14_mga16_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    # Run
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'GGA14_MGA16'
    cat = 'Real'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_REAL_HS12_PT12:
    par_main = '-rm 14,0,0.001,0.95 -f 9000 -d 500 -th 1.9 -m 100000'
    par_viz = '-l 1 -p 1 -vv -rn "HS 12" -tn "PT 12" ' + \
        '-rt 15000000 -tt 15000000 -stat -o HS12_PT12.svg'
    ref = real_hs12_pt12_path_ref + real_hs12_pt12_ref_name
    tar = real_hs12_pt12_path_tar + real_hs12_pt12_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    # Run
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'HS12_PT12'
    cat = 'Real'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_REAL_PXO99A_MAFF311018:
    par_main = '-rm 13,0,0.005,1 -f 150 -m 10000 -d 1000 -th 1.55 -ar'
    par_viz = '-l 6 -vv -p 1 -rt 500000 -rn PXO99A -tn "MAFF 311018" ' + \
        '-stat -o PXO99A_MAFF_311018.svg'
    ref = real_PXO99A_MAFF311018_path_ref + real_PXO99A_MAFF311018_ref_name
    tar = real_PXO99A_MAFF311018_path_tar + real_PXO99A_MAFF311018_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    # Run
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'PXO99A_MAFF311018'
    cat = 'Real'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_SYNTH_COMPARE_SMASH:
    # Smash++
    par_main = '-th 1.7 -l 3 -f 1000 -d 10 -m 1 -sf'
    par_viz = '-p 1 -l 1 -w 13 -rn Ref -tn Tar -rt 100000 -tt 100000 ' + \
        '-stat -o CompSmash.svg'
    ref = path_data_synth + synth_comp_smash_ref_name
    tar = path_data_synth + synth_comp_smash_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    ## Run
    execute(cmd_main)
    execute(cmd_viz)
    ## Bench
    bench = True
    method = 'Smash++'
    dataset = 'CompSynth'
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

    # Smash
    par = '-t 1.7 -c 14 -d 9 -w 5000 -m 1 -nd '
    ref_main = path_data_synth + synth_comp_smash_ref_name
    tar_main = path_data_synth + synth_comp_smash_tar_name
    ref = synth_comp_smash_ref_name
    tar = synth_comp_smash_tar_name
    cmd = time_exe + log_smash + ' ' + \
        smash + ' ' + par + ' ' + ref + ' ' + tar
    ## Run
    shutil.copyfile(ref_main, ref)
    shutil.copyfile(tar_main, tar)
    execute(cmd)
    os.remove(ref)
    os.remove(tar)
    remove_all_ext(current_dir, 'ext')
    remove_all_ext(current_dir, 'rev')
    remove_all_ext(current_dir, 'inf')
    remove(current_dir, '*.sys*x')
    ## Bench
    method = 'Smash'
    mem = calc_mem(log_smash)
    elapsed = calc_elapsed(log_smash)
    user_time = calc_user_time(log_smash)
    system_time = calc_system_time(log_smash)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_REAL_COMPARE_SMASH:
    path_ref = path_data + 'fungi' + sep + 'Saccharomyces_cerevisiae' + sep
    path_tar = path_data + 'fungi' + sep + 'Saccharomyces_paradoxus' + sep

    # Smash++
    par_main = '-th 1.85 -l 3 -f 370 -d 100 -ar -sf'
    par_viz = '-p 1 -l 1 -w 13 -rn Sc.VII -tn Sp.VII -stat ' + \
        '-o Sc_Sp_smash.svg'
    ref = path_ref + real_comp_smash_ref_name
    tar = path_tar + real_comp_smash_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    ## Run
    execute(cmd_main)
    execute(cmd_viz)
    ## Bench
    bench = True
    method = 'Smash++'
    dataset = 'CompReal'
    cat = 'Real'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

    # Smash
    par = '-t 1.85 -c 14 -d 99 -w 15000 -m 1 -nd '
    ref_new_smash = 'Sc' + real_comp_smash_ref_name
    tar_new_smash = 'Sp' + real_comp_smash_tar_name
    ref_main = path_ref + real_comp_smash_ref_name
    tar_main = path_tar + real_comp_smash_tar_name
    ref = ref_new_smash
    tar = tar_new_smash
    cmd = time_exe + log_smash + ' ' + \
        smash + ' ' + par + ' ' + ref + ' ' + tar
    ## Run
    shutil.copyfile(ref_main, ref)
    shutil.copyfile(tar_main, tar)
    execute(cmd)
    os.remove(ref)
    os.remove(tar)
    remove_all_ext(current_dir, 'ext')
    remove_all_ext(current_dir, 'rev')
    remove_all_ext(current_dir, 'inf')
    remove(current_dir, '*.sys*x')
    ## Bench
    method = 'Smash'
    mem = calc_mem(log_smash)
    elapsed = calc_elapsed(log_smash)
    user_time = calc_user_time(log_smash)
    system_time = calc_system_time(log_smash)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_SYNTH_PERM_ORIGINAL:
    par_main = '-l 0 -f 10 -d 3000'
    par_viz = '-p 1 -l 6 -w 13 -s 35 -vv -rt 500000 -tt 500000 -o Perm.svg'
    ref = path_data_synth + synth_perm_ref_name
    tar = path_data_synth + synth_perm_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    # Run
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'PermOrig'
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_SYNTH_PERM_450000:
    block_size = '450000'
    ref_name = synth_perm_ref_name + block_size
    par_main = '-l 0 -f 25 -d 3000 -ar'
    par_viz = '-p 1 -l 6 -w 13 -s 35 -vv -rt 500000 -tt 500000 ' + \
        '-o Perm_' + block_size + '.svg'
    ref = path_data_synth + ref_name
    tar = path_data_synth + synth_perm_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    seed = '6041'
    # Run
    execute(goose_permuteseqbyblocks + '-bs ' + block_size + ' -s ' + seed +
            ' < ' + path_data_synth + synth_perm_ref_name +
            ' > ' + path_data_synth + ref_name)
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'Perm' + block_size
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_SYNTH_PERM_30000:
    block_size = '30000'
    ref_name = synth_perm_ref_name + block_size
    par_main = '-l 0 -f 75 -d 1500 -ar'
    par_viz = '-p 1 -l 6 -w 13 -s 35 -vv -rt 500000 -tt 500000 ' + \
        '-o Perm_' + block_size + '.svg'
    ref = path_data_synth + ref_name
    tar = path_data_synth + synth_perm_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    seed = '328914'
    # Run
    execute(goose_permuteseqbyblocks + '-bs ' + block_size + ' -s ' + seed +
            ' < ' + path_data_synth + synth_perm_ref_name +
            ' > ' + path_data_synth + ref_name)
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'Perm' + block_size
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_SYNTH_PERM_1000:
    block_size = '1000'
    ref_name = synth_perm_ref_name + block_size
    par_main = '-l 0 -f 25 -d 300 -ar'
    par_viz = '-p 1 -l 6 -w 13  -rt 500000 -tt 500000 ' + \
        '-o Perm_' + block_size + '.svg'
    ref = path_data_synth + ref_name
    tar = path_data_synth + synth_perm_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    seed = '564283'
    # Run
    execute(goose_permuteseqbyblocks + '-bs ' + block_size + ' -s ' + seed +
            ' < ' + path_data_synth + synth_perm_ref_name +
            ' > ' + path_data_synth + ref_name)
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'Perm' + block_size
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])

if RUN_SYNTH_PERM_30:
    block_size = '30'
    ref_name = synth_perm_ref_name + block_size
    par_main = '-l 0 -f 250 -d 1 -ar'
    par_viz = '-p 1 -l 6 -w 13 -s 35 -vv -rt 500000 -tt 500000 ' + \
        '-o Perm_' + block_size + '.svg'
    ref = path_data_synth + ref_name
    tar = path_data_synth + synth_perm_tar_name
    cmd_main = time_exe + log_main + ' ' + smashpp_exe + \
        ' -r ' + ref + ' -t ' + tar + ' ' + par_main
    cmd_viz = time_exe + log_viz + ' ' + smashpp_exe + ' -viz ' + par_viz + \
        ' ' + bare_name(ref) + '.' + bare_name(tar) + '.pos'
    seed = '900123'
    # Run
    execute(goose_permuteseqbyblocks + '-bs ' + block_size + ' -s ' + seed +
            ' < ' + path_data_synth + synth_perm_ref_name +
            ' > ' + path_data_synth + ref_name)
    execute(cmd_main)
    execute(cmd_viz)
    # Bench
    bench = True
    method = 'Smash++'
    dataset = 'Perm' + block_size
    cat = 'Synthetic'
    size = file_size(ref) + file_size(tar)
    mem = calc_mem(log_main, log_viz)
    elapsed = calc_elapsed(log_main, log_viz)
    user_time = calc_user_time(log_main, log_viz)
    system_time = calc_system_time(log_main, log_viz)
    bench_result.append([method, dataset, cat, size, mem,
                         elapsed, user_time, system_time])


# def run_bench(func, method, dataset, cat, size):
#     import memory_profiler
#     import time
#     start_time = time.perf_counter()
#     memory = memory_profiler.memory_usage(func)
#     end_time = time.perf_counter()
#     elapsed = f"{end_time - start_time:.2f}"
#     max_memory = f"{max(memory):.2f}"
#     bench_result.append([method, dataset, cat, size, elapsed, max_memory])

# def run_bench(method, dataset, cat, size, func, arg=''):
#     import time
#     start_time = time.perf_counter()

#     def start(method, interval=0.001):
#         method_process = 'smashpp'
#         if method == 'Smash':
#             method_process = 'smash'
#         cmd_bash = \
#             'echo "0" > mem_ps; \
#             while true; do \
#                 ps -T aux | grep ' + method_process + ' | grep -v "grep --color=auto" \
#                     | awk \'{print $7}\' | sort -V | tail -n 1 >> mem_ps; \
#                 sleep ' + str(interval) + '; \
#             done'
#         os.popen(cmd_bash)

#     def stop():
#         cmd_bash = \
#             'kill $! >/dev/null 2>&1; \
#             cat mem_ps | sort -V | tail -n 1 > peak_mem; \
#             rm -f mem_ps; \
#             kill `ps -T aux | grep dash | grep -v "grep --color=auto" | awk \'{print $2}\'`;'
#         os.popen(cmd_bash)

#     # start(method=method)
#     memory = memory_profiler.memory_usage(func)
#     # stop()
#     end_time = time.perf_counter()
#     elapsed = f"{end_time - start_time:.2f}"
#     max_memory = f"{max(memory):.2f}"
#     bench_result.append([method, dataset, cat, size, elapsed, max_memory])


if bench:
    with open('bench.csv', 'w') as bench_file:
        writer = csv.writer(bench_file)
        writer.writerow(['Method', 'Dataset', 'Cat', 'Size.B',
                         'Memory.B', 'Elapsed.s', 'User.s', 'System.s'])
        writer.writerows(bench_result)
    remove_path(log_main)
    remove_path(log_viz)
    remove_path(log_smash)
    # Move obtained results to the result/ directory
    make_path('result')
    for file in os.listdir(current_dir):
        if file.endswith('.csv') or file.endswith('.svg') or \
                file.endswith('.pos') or file.endswith('.fil'):
            shutil.move(file, 'result/')
