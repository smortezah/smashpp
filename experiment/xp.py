'''
Smash++
Morteza Hosseini, Diogo Pratas, Armando J. Pinho
{seyedmorteza,pratas,ap}@ua.pt
Copyright (C) 2018-2019, IEETA/DETI, University of Aveiro, Portugal
'''

'''
USER CAN CHANGE HERE BY SWITCHING False/True
'''
# Resolve dependencies
from memory_profiler import memory_usage
from pathlib import Path
from shutil import copyfile
import psutil
import csv
import time
import os
GET_GOOSE = False

# Make synthetic dataset
MAKE_SYNTH_SMALL = False
MAKE_SYNTH_MEDIUM = False
MAKE_SYNTH_LARGE = False
MAKE_SYNTH_XLARGE = False
MAKE_SYNTH_MUTATE = False
MAKE_SYNTH_COMPARE_SMASH = False

# Run
RUN_SYNTH_SMALL = False
RUN_SYNTH_MEDIUM = False
RUN_SYNTH_LARGE = False
RUN_SYNTH_XLARGE = False
RUN_SYNTH_MUTATE = False
RUN_REAL_GGA18_MGA20 = False
RUN_REAL_GGA14_MGA16 = False
RUN_REAL_HS12_PT12 = False
RUN_REAL_PXO99A_MAFF311018 = False
RUN_COMP_SMASH_SYNTH = False  # Compare to Smash on synthetic data
RUN_COMP_SMASH_REAL = False  # Compare to Smash on real data
RUN_SYNTH_PERM = True

# Benchmark
BENCH_SYNTH_SMALL = False
BENCH_SYNTH_MEDIUM = False
BENCH_SYNTH_LARGE = False
BENCH_SYNTH_XLARGE = False
BENCH_SYNTH_MUTATE = False
BENCH_REAL_GGA18_MGA20 = False
BENCH_REAL_GGA14_MGA16 = False
BENCH_REAL_HS12_PT12 = False
BENCH_REAL_PXO99A_MAFF = False


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
real_PXO99A_MAFF_ref_name = 'PXO99A.seq'
real_PXO99A_MAFF_tar_name = 'MAFF_311018.seq'
real_PXO99A_MAFF_path_ref = path_data + 'bacteria' + sep + \
    'Xanthomonas_oryzae_pv_oryzae' + sep
real_PXO99A_MAFF_path_tar = real_PXO99A_MAFF_path_ref
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


def remove(dir, pattern):
    for p in Path(dir).glob(pattern):
        p.unlink()


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
# os.popen('sudo chmod 777 smashpp-inv-rep').read()


def make_synth_small():  # Sizes: ref:1,500, tar:1,500
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


def make_synth_medium():  # Sizes: ref:100,000, tar:100,000
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
    copyfile('r_c', 't_a')
    execute(goose_mutatedna + '-mr 0.03 < r_d > t_b')
    cat(['t_a', 't_b', 't_c', 't_d'], path_data_synth + 'TarM')


def make_synth_large():  # Sizes: ref:5,000,000, tar:5,000,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 25000 -s 10101  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 25000 -s 10  r_b')
    cat(['r_a', 'r_b'], path_data_synth + 'RefL')

    execute(smashpp_inv_rep + 'r_a t_b')
    execute(goose_mutatedna + '-mr 0.02 < r_b > t_a')
    cat(['t_a', 't_b'], path_data_synth + 'TarL')


def make_synth_xlarge():  # Sizes: ref:100,000,000, tar:100,000,000
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
    copyfile('r_d', 't_c')
    execute(smashpp_inv_rep + 'r_b t_d')
    cat(['t_a', 't_b', 't_c', 't_d'], path_data_synth + 'TarXL')


def make_synth_mutate():  # Sizes: ref:60,000, tar:60,000. Mutation up to 60%
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


def make_synth_compare_smash():  # Sizes: ref:1,000,000, tar:1,000,000
    if os.path.exists(path_data_synth + "RefComp"):
        os.remove(path_data_synth + "RefComp")
    if os.path.exists(path_data_synth + "TarComp"):
        os.remove(path_data_synth + "TarComp")

    for i in range(0, 9 + 1):
        execute(goose_fastqsimulation + synth_common_par +
                ' -s ' + str(i * 10 + 1) +
                '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 1000 r_' + str(i))
        append('r_' + str(i), path_data_synth + 'RefComp')
    copyfile('r_0', 't_0')
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


if MAKE_SYNTH_SMALL:
    make_synth_small()

if MAKE_SYNTH_MEDIUM:
    make_synth_medium()

if MAKE_SYNTH_LARGE:
    make_synth_large()

if MAKE_SYNTH_XLARGE:
    make_synth_xlarge()

if MAKE_SYNTH_MUTATE:
    make_synth_mutate()

if MAKE_SYNTH_COMPARE_SMASH:
    make_synth_compare_smash()

remove_all_start(current_dir, "r_")
remove_all_start(current_dir, "t_")


'''
Run
'''
def run_smashpp(ref, tar, par_main, par_viz):
    ref_name = os.path.basename(ref)
    tar_name = os.path.basename(tar)
    execute(smashpp_exe + ' -r ' + ref + ' -t ' + tar + ' ' + par_main)
    execute(smashpp_exe + ' -viz ' + par_viz + ' ' +
            ref_name + '.' + tar_name + '.pos')


def run_synth_small():
    par_main = '-l 3 -d 1 -f 100'
    par_viz = '-p 1 -rt 150 -tt 150 -l 1 -w 13 -vv -stat -o S.svg'
    run_smashpp(path_data_synth + synth_small_ref_name,
                path_data_synth + synth_small_tar_name, par_main, par_viz)


def run_synth_medium():
    par_main = '-l 3 -d 100 -f 50'
    par_viz = '-p 1 -l 1 -w 13 -vv -stat -o M.svg'
    run_smashpp(path_data_synth + synth_medium_ref_name,
                path_data_synth + synth_medium_tar_name, par_main, par_viz)


def run_synth_large():
    par_main = '-l 3 -d 100 -f 135'
    par_viz = '-p 1 -l 1 -w 13 -vv -stat -o L.svg'
    run_smashpp(path_data_synth + synth_large_ref_name,
                path_data_synth + synth_large_tar_name, par_main, par_viz)


def run_synth_xlarge():
    par_main = '-l 3 -d 100 -f 275'
    par_viz = '-p 1 -l 1 -w 13 -vv -stat -o XL.svg'
    run_smashpp(path_data_synth + synth_xlarge_ref_name,
                path_data_synth + synth_xlarge_tar_name, par_main, par_viz)


def run_synth_mutate():
    par_main = '-th 1.97 -l 3 -d 600 -f 100 -m 15000'
    par_viz = '-p 1 -l 1 -w 13 -rt 5000 -tt 5000 -vv -stat -o Mut.svg'
    run_smashpp(path_data_synth + synth_mutate_ref_name,
                path_data_synth + synth_mutate_tar_name, par_main, par_viz)


def run_real_GGA18_MGA20():
    par_main = '-rm 14,0,0.005,0.95/5,0,1,0.95 -f 130 -m 500000 -d 2200 ' + \
        '-th 1.9'
    par_viz = '-l 1 -p 1 -vv -tc 6 -rn "GGA 18" -tn "MGA 20" ' + \
        '-stat -o GGA18_MGA20.svg'
    run_smashpp(real_gga18_mga20_path_ref + real_gga18_mga20_ref_name,
                real_gga18_mga20_path_tar + real_gga18_mga20_tar_name,
                par_main, par_viz)


def run_real_GGA14_MGA16():
    par_main = '-rm 14,0,0.005,0.95/5,0,0.99,0.95 -f 200 -d 1500 -th 1.95 ' + \
        '-e 1.95 -m 400000'
    par_viz = '-l 1 -vv -p 1 -rn "GGA 14" -tn "MGA 16" ' + \
        '-rt 1500000 -tt 1500000 -stat -o GGA14_MGA16.svg'
    run_smashpp(real_gga14_mga16_path_ref + real_gga14_mga16_ref_name,
                real_gga14_mga16_path_tar + real_gga14_mga16_tar_name,
                par_main, par_viz)


def run_real_HS12_PT12():
    par_main = '-rm 14,0,0.001,0.95 -f 9000 -d 500 -th 1.9 -m 100000'
    par_viz = '-l 1 -p 1 -vv -rn "HS 12" -tn "PT 12" ' + \
        '-rt 15000000 -tt 15000000 -stat -o HS12_PT12.svg'
    run_smashpp(real_hs12_pt12_path_ref + real_hs12_pt12_ref_name,
                real_hs12_pt12_path_tar + real_hs12_pt12_tar_name,
                par_main, par_viz)


def run_real_PXO99A_MAFF311018():
    par_main = '-rm 13,0,0.005,1 -f 150 -m 10000 -d 1000 -th 1.55 -ar'
    par_viz = '-l 6 -vv -p 1 -rt 500000 -rn PXO99A -tn "MAFF 311018" ' + \
        '-stat -o PXO99A_MAFF_311018.svg'
    run_smashpp(real_PXO99A_MAFF_path_ref + real_PXO99A_MAFF_ref_name,
                real_PXO99A_MAFF_path_tar + real_PXO99A_MAFF_tar_name,
                par_main, par_viz)


def run_smash(ref_main, tar_main, ref, tar, par, curr_dir):
    copyfile(ref_main, ref)
    copyfile(tar_main, tar)
    execute(smash + ' ' + par + ' ' + ref + ' ' + tar)
    os.remove(ref)
    os.remove(tar)
    remove_all_ext(curr_dir, 'ext')
    remove_all_ext(curr_dir, 'rev')
    remove_all_ext(curr_dir, 'inf')
    remove(curr_dir, '*.sys*x')


def run_comp_Smash_synth():
    # Smash++
    par_main = '-th 1.7 -l 3 -f 1000 -d 10 -m 1 -sf'
    par_viz = '-p 1 -l 1 -w 13 -rn Ref -tn Tar -rt 100000 -tt 100000 ' + \
        '-stat -o CompSmash.svg'
    run_smashpp(path_data_synth + synth_comp_smash_ref_name,
                path_data_synth + synth_comp_smash_tar_name, par_main, par_viz)

    # Smash
    par = '-t 1.7 -c 14 -d 9 -w 5000 -m 1 -nd '
    run_smash(path_data_synth + synth_comp_smash_ref_name,
              path_data_synth + synth_comp_smash_tar_name,
              synth_comp_smash_ref_name, synth_comp_smash_tar_name, par,
              current_dir)


def run_comp_Smash_real():
    path_ref = path_data + 'fungi' + sep + 'Saccharomyces_cerevisiae' + sep
    path_tar = path_data + 'fungi' + sep + 'Saccharomyces_paradoxus' + sep

    # Smash++
    par_main = '-th 1.85 -l 3 -f 370 -d 100 -ar -sf'
    par_viz = '-p 1 -l 1 -w 13 -rn Sc.VII -tn Sp.VII -stat -o Sc_Sp_smash.svg'
    run_smashpp(path_ref + real_comp_smash_ref_name,
                path_tar + real_comp_smash_tar_name, par_main, par_viz)

    # Smash
    par = '-t 1.85 -c 14 -d 99 -w 15000 -m 1 -nd '
    ref_new_smash = 'Sc' + real_comp_smash_ref_name
    tar_new_smash = 'Sp' + real_comp_smash_tar_name
    run_smash(path_ref + real_comp_smash_ref_name,
              path_tar + real_comp_smash_tar_name, ref_new_smash,
              tar_new_smash, par, current_dir)


if RUN_SYNTH_SMALL:
    run_synth_small()

if RUN_SYNTH_MEDIUM:
    run_synth_medium()

if RUN_SYNTH_LARGE:
    run_synth_large()

if RUN_SYNTH_XLARGE:
    run_synth_xlarge()

if RUN_SYNTH_MUTATE:
    run_synth_mutate()

if RUN_REAL_GGA18_MGA20:
    run_real_GGA18_MGA20()

if RUN_REAL_GGA14_MGA16:
    run_real_GGA14_MGA16()

if RUN_REAL_HS12_PT12:
    run_real_HS12_PT12()

if RUN_REAL_PXO99A_MAFF311018:
    run_real_PXO99A_MAFF311018()

if RUN_COMP_SMASH_SYNTH:
    run_comp_Smash_synth()

if RUN_COMP_SMASH_REAL:
    run_comp_Smash_real()


'''
Benchmark
'''
bench_result = []  # Name, Category, Size, Time, Memory


def run_bench(func, name, cat, size):
    start_time = time.perf_counter()
    memory = memory_usage(func)
    end_time = time.perf_counter()
    elapsed = f"{end_time - start_time:.2f}"
    max_memory = f"{max(memory):.2f}"
    bench_result.append([name, cat, size, elapsed, max_memory])


bench = False
if BENCH_SYNTH_SMALL:
    bench = True
    name = 'Small'
    cat = 'Synthetic'
    size = os.path.getsize(path_data_synth + synth_small_ref_name) + \
        os.path.getsize(path_data_synth + synth_small_tar_name)
    run_bench(run_synth_small, name, cat, size)

if BENCH_SYNTH_MEDIUM:
    bench = True
    name = 'Medium'
    cat = 'Synthetic'
    size = os.path.getsize(path_data_synth + synth_medium_ref_name) + \
        os.path.getsize(path_data_synth + synth_medium_tar_name)
    run_bench(run_synth_medium, name, cat, size)

if BENCH_SYNTH_LARGE:
    bench = True
    name = 'Large'
    cat = 'Synthetic'
    size = os.path.getsize(path_data_synth + synth_large_ref_name) + \
        os.path.getsize(path_data_synth + synth_large_tar_name)
    run_bench(run_synth_large, name, cat, size)

if BENCH_SYNTH_XLARGE:
    bench = True
    name = 'XLarge'
    cat = 'Synthetic'
    size = os.path.getsize(path_data_synth + synth_xlarge_ref_name) + \
        os.path.getsize(path_data_synth + synth_xlarge_tar_name)
    run_bench(run_synth_xlarge, name, cat, size)

if BENCH_SYNTH_MUTATE:
    bench = True
    name = 'Mutate'
    cat = 'Synthetic'
    size = os.path.getsize(path_data_synth + synth_mutate_ref_name) + \
        os.path.getsize(path_data_synth + synth_mutate_tar_name)
    run_bench(run_synth_mutate, name, cat, size)

if BENCH_REAL_GGA18_MGA20:
    bench = True
    name = 'GGA18_MGA20'
    cat = 'Real'
    size = os.path.getsize(real_gga18_mga20_path_ref +
                           real_gga18_mga20_ref_name) + \
        os.path.getsize(real_gga18_mga20_path_tar + real_gga18_mga20_tar_name)
    run_bench(run_real_gga18_mga20, name, cat, size)

if BENCH_REAL_GGA14_MGA16:
    bench = True
    name = 'GGA14_MGA16'
    cat = 'Real'
    size = os.path.getsize(real_gga14_mga16_path_ref +
                           real_gga14_mga16_ref_name) + \
        os.path.getsize(real_gga14_mga16_path_tar + real_gga14_mga16_tar_name)
    run_bench(run_real_gga14_mga16, name, cat, size)

if BENCH_REAL_HS12_PT12:
    bench = True
    name = 'HS12_PT12'
    cat = 'Real'
    size = os.path.getsize(real_hs12_pt12_path_ref +
                           real_hs12_pt12_ref_name) + \
        os.path.getsize(real_hs12_pt12_path_tar + real_hs12_pt12_tar_name)
    run_bench(run_real_hs12_pt12, name, cat, size)

if BENCH_REAL_PXO99A_MAFF:
    bench = True
    name = 'PXO99A_MAFF'
    cat = 'Real'
    size = os.path.getsize(real_PXO99A_MAFF_path_ref +
                           real_PXO99A_MAFF_ref_name) + \
        os.path.getsize(real_PXO99A_MAFF_path_tar + real_PXO99A_MAFF_tar_name)
    run_bench(run_real_PXO99A_MAFF, name, cat, size)

if bench:
    with open('bench.csv', 'w') as bench_file:
        writer = csv.writer(bench_file)
        writer.writerow(['Name', 'Cat', 'Size.B', 'Time.s', 'Memory.MB'])
        writer.writerows(bench_result)


# todo
if RUN_SYNTH_PERM:
    original = False
    perm_450000 = False
    perm_30000 = False
    perm_1500 = True

    if original:
        par_main = '-l 0 -f 10 -d 3000'
        par_viz = '-p 1 -l 6 -w 13 -vv -o Perm.svg'
        run_smashpp(path_data_synth + synth_perm_ref_name,
                    path_data_synth + synth_perm_tar_name, par_main, par_viz)

    if perm_450000:
        block_size = 450000
        ref_name = synth_perm_ref_name + str(block_size)
        par_main = '-l 0 -f 25 -d 3000 -ar'
        par_viz = '-p 1 -l 6 -w 13 -vv -o Perm_450000.svg'
        execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
                '-s 6041 < ' + path_data_synth + synth_perm_ref_name +
                ' > ' + path_data_synth + ref_name)
        run_smashpp(path_data_synth + ref_name,
                    path_data_synth + synth_perm_tar_name, par_main, par_viz)

    if perm_30000:
        block_size = 30000
        ref_name = synth_perm_ref_name + str(block_size)
        par_main = '-l 0 -f 75 -d 1500 -ar'
        par_viz = '-p 1 -l 6 -w 13 -vv -o Perm_30000.svg'
        execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
                '-s 328914 < ' + path_data_synth + synth_perm_ref_name +
                ' > ' + path_data_synth + ref_name)
        run_smashpp(path_data_synth + ref_name,
                    path_data_synth + synth_perm_tar_name, par_main, par_viz)

    if perm_1500:
        block_size = 1000
        ref_name = synth_perm_ref_name + str(block_size)
        par_main = '-l 0 -f 300 -d 600 -ar'
        par_viz = '-p 1 -l 6 -w 13 -vv -o Perm_1000.svg'
        execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
                '-s 564283 < ' + path_data_synth + synth_perm_ref_name +
                ' > ' + path_data_synth + ref_name)
        run_smashpp(path_data_synth + ref_name,
                    path_data_synth + synth_perm_tar_name, par_main, par_viz)

#     block_size = 40
#     ref_perm = ref + str(block_size)
#     execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
#             '-s 564283 < ' + path_data_synth + ref +
#             ' > ' + path_data_synth + ref_perm)
#     execute(smashpp_exe + '-r ' + path_data_synth + ref_perm +
#             ' -t ' + path_data_synth + tar +
#             ' -th 1.5 -rm 14,0,0.001,0.9 -f 3000 -d 1 -ar ')
#     execute(smashpp_exe + '-viz -rn Ref_perm -tn Tar ' + viz_par +
#             '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

    # Permutated
#     block_size = 20
#     ref_perm = ref + str(block_size)
# #     execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
# #             '-s 165604 < ' + path_data_synth + ref + ' > ' +
# #             path_data_synth + ref_perm)

# #     copyfile(path_data_synth + ref_perm, ref_perm)
# #     copyfile(path_data_synth + tar, tar)
# #     execute(smash + ' -t 1.9 -c 14 -w 100 -d 1 ' + ref_perm + ' ' + tar)
# #     os.remove(ref_perm)
# #     os.remove(tar)
# #     remove_all_ext(current_dir, 'ext')
# #     remove_all_ext(current_dir, 'rev')
# #     remove(current_dir, '*.sys*')

#     execute(smashpp_exe + '-r ' + path_data_synth + ref_perm +
#             ' -t ' + path_data_synth + tar +
#             ' -th 5 -rm 14,0,0.001,0.95:3,0,0.001,0.95 -f 3000 -d 1 -ar ' +
#             '-nr -sf ')
#     execute(smashpp_exe + '-viz -rn Ref_perm -tn Tar ' + viz_par +
#             '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')