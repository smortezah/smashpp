import os
import time
import csv
import psutil
from shutil import copyfile
from pathlib import Path
from memory_profiler import memory_usage

get_goose = False  # Get dependencies
make_synth_data = False  # Make synthetic dataset

run = False  # Run on synthetic and real dataset

# sim_permute = False
# sim_permute_smash = False

# Benchmark
bench = False

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
current_dir = os.getcwd()
path_data_synth = 'dataset' + sep + 'sim' + sep
path_data_real = 'dataset' + sep
path_data_permute = 'dataset' + sep + 'permute' + sep
path_bin = 'bin' + sep
goose_fastqsimulation = path_bin + 'goose-fastqsimulation '
goose_mutatedna = path_bin + 'goose-mutatedna '
goose_permuteseqbyblocks = path_bin + 'goose-permuteseqbyblocks '
smashpp = '..' + sep + 'smashpp '
smashpp_inv_rep = path_bin + 'smashpp-inv-rep '
smash = path_bin + 'smash '
synth_common_par = '-eh -eo -es -edb -rm 0 '
# sim_common_par = ' -w 10 -s 19 -vv '
sim_common_par = ' -vv '

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
real_comp_smash_ref_name = 'VII.seq'
real_comp_smash_tar_name = 'VII.seq'
real_PXO99A_MAFF_ref_name = 'PXO99A.seq'
real_PXO99A_MAFF_tar_name = 'MAFF_311018.seq'
real_PXO99A_MAFF_path_ref = path_data_real + 'bacteria' + \
    sep + 'Xanthomonas_oryzae_pv_oryzae' + sep
real_PXO99A_MAFF_path_tar = real_PXO99A_MAFF_path_ref
real_gga18_mga20_ref_name = '18.seq'
real_gga18_mga20_tar_name = '20.seq'
real_gga18_mga20_path_ref = path_data_real + 'bird' + sep + \
    'Gallus_gallus' + sep
real_gga18_mga20_path_tar = path_data_real + 'bird' + sep + \
    'Meleagris_gallopavo' + sep
real_gga14_mga16_ref_name = '14.seq'
real_gga14_mga16_tar_name = '16.seq'
real_gga14_mga16_path_ref = path_data_real + 'bird' + sep + \
    'Gallus_gallus' + sep
real_gga14_mga16_path_tar = path_data_real + 'bird' + sep + \
    'Meleagris_gallopavo' + sep
real_hs12_pt12_path_ref = path_data_real + 'mammalia' + sep + \
    'Homo_sapiens' + sep
real_hs12_pt12_path_tar = path_data_real + 'mammalia' + sep + \
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
    with open(file_out_name, 'a') as file_out:
        with open(file_in_name) as file_in:
            for line in file_in:
                file_out.write(line)


def remove_all_ext(directory, extension):
    for item in os.listdir(directory):
        if item.endswith(extension):
            os.remove(os.path.join(directory, item))


def remove(dir, pattern):
    for p in Path(dir).glob(pattern):
        p.unlink()


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


def run_smashpp(path_ref, path_tar, ref_name, tar_name, par_main, par_viz):
    execute(smashpp + ' -r ' + path_ref + ref_name + ' -t ' +
            path_tar + tar_name + ' ' + par_main)
    execute(smashpp + ' -viz ' + par_viz + ' ' +
            ref_name + '.' + tar_name + '.pos')


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
def make_synth_small():  # sizes: ref:1,500, tar:1,500
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


def make_synth_medium():  # sizes: ref:100,000, tar:100,000
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


def make_synth_large():  # sizes: ref:5,000,000, tar:5,000,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 25000 -s 10101  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 25000 -s 10  r_b')
    cat(['r_a', 'r_b'], path_data_synth + 'RefL')

    execute(smashpp_inv_rep + 'r_a t_b')
    execute(goose_mutatedna + '-mr 0.02 < r_b > t_a')
    cat(['t_a', 't_b'], path_data_synth + 'TarL')


def make_synth_xlarge():  # sizes: ref:100,000,000, tar:100,000,000
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


def make_synth_mutate():  # sizes: ref:60,000, tar:60,000. Mutation up to 60%
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


def make_synth_compare_smash():  # sizes: ref:1,000,000, tar:1,000,000
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


if make_synth_data:
    make_synth_small()
    make_synth_medium()
    make_synth_large()
    make_synth_xlarge()
    make_synth_mutate()
    make_synth_compare_smash()


for file in os.listdir(current_dir):
    if file.startswith("r_"):
        os.remove(os.path.join(current_dir, file))
    if file.startswith("t_"):
        os.remove(os.path.join(current_dir, file))


def run_synth_small():
    par_main = '-l 3 -d 1 -f 100 -dp'
    par_viz = '-p 1 -rt 150 -tt 150 -l 1 -w 13 -vv -stat -o S.svg'
    run_smashpp(path_data_synth, path_data_synth, synth_small_ref_name,
                synth_small_tar_name, par_main, par_viz)


def run_synth_medium():
    par_main = '-l 3 -d 100 -f 50 -dp'
    par_viz = '-p 1 -l 1 -w 13 -vv -stat -o M.svg'
    run_smashpp(path_data_synth, path_data_synth, synth_medium_ref_name,
                synth_medium_tar_name, par_main, par_viz)


def run_synth_large():
    par_main = '-l 3 -d 100 -f 135 -dp'
    par_viz = '-p 1 -l 1 -w 13 -vv -stat -o L.svg'
    run_smashpp(path_data_synth, path_data_synth, synth_large_ref_name,
                synth_large_tar_name, par_main, par_viz)


def run_synth_xlarge():
    par_main = '-l 3 -d 100 -f 275 -dp'
    par_viz = '-p 1 -l 1 -w 13 -vv -stat -o XL.svg'
    run_smashpp(path_data_synth, path_data_synth, synth_xlarge_ref_name,
                synth_xlarge_tar_name, par_main, par_viz)


def run_synth_mutate():
    par_main = '-th 1.97 -l 3 -d 600 -f 100 -m 15000 -dp'
    par_viz = '-p 1 -l 1 -w 13 -rt 5000 -tt 5000 -vv -stat -o Mut.svg'
    run_smashpp(path_data_synth, path_data_synth, synth_mutate_ref_name,
                synth_mutate_tar_name, par_main, par_viz)


def run_comp_smash():
    a = True
    b = True

    if a:
        # Smash++
        par_main = '-th 1.7 -l 3 -f 1000 -d 10 -m 1 -dp -sf'
        par_viz = '-p 1 -l 1 -w 13 -rn Ref -tn Tar -rt 100000 -tt 100000 ' + \
            '-stat -o CompSmash.svg'
        run_smashpp(path_data_synth, path_data_synth,
                    synth_comp_smash_ref_name, synth_comp_smash_tar_name,
                    par_main, par_viz)

        # Smash
        par = '-t 1.7 -c 14 -d 9 -w 5000 -m 1 -nd '
        run_smash(path_data_synth + synth_comp_smash_ref_name,
                  path_data_synth + synth_comp_smash_tar_name,
                  synth_comp_smash_ref_name, synth_comp_smash_tar_name, par,
                  current_dir)

    if b:
        path_ref = path_data_real + 'fungi' + sep + \
            'Saccharomyces_cerevisiae' + sep
        path_tar = path_data_real + 'fungi' + sep + \
            'Saccharomyces_paradoxus' + sep

        # Smash++
        par_main = '-th 1.85 -l 3 -f 370 -d 100 -ar -dp -sf'
        par_viz = '-p 1 -l 1 -w 13 -rn Sc.VII -tn Sp.VII -stat ' + \
            ' -o Sc_Sp_smash.svg'
        run_smashpp(path_ref, path_tar, real_comp_smash_ref_name,
                    real_comp_smash_tar_name, par_main, par_viz)

        # Smash
        par = '-t 1.85 -c 14 -d 99 -w 15000 -m 1 -nd '
        ref_new_smash = 'Sc' + real_comp_smash_ref_name
        tar_new_smash = 'Sp' + real_comp_smash_tar_name
        run_smash(path_ref + real_comp_smash_ref_name,
                  path_tar + real_comp_smash_tar_name,
                  ref_new_smash, tar_new_smash, par, current_dir)


def run_real_PXO99A_MAFF():
    par_main = '-rm 13,0,0.005,1 -f 150 -m 10000 -d 1000 -th 1.55 -ar -dp'
    par_viz = '-l 6 -s 10 -w 8 -p 1 -rt 500000 ' + \
        '-rn PXO99A -tn "MAFF 311018" -stat -o PXO99A_MAFF_311018.svg'
    run_smashpp(real_PXO99A_MAFF_path_ref, real_PXO99A_MAFF_path_tar,
                real_PXO99A_MAFF_ref_name, real_PXO99A_MAFF_tar_name,
                par_main, par_viz)


def run_real_gga18_mga20():
    par_main = '-rm 14,0,0.005,0.95/5,0,1,0.95 -f 130 -m 500000 -d 2200 ' + \
        '-th 1.9 -dp'
    par_viz = '-l 1 -p 1 -vv -tc 6 -rn "GGA 18" -tn "MGA 20" -stat ' + \
        '-o GGA18_MGA20.svg'
    run_smashpp(real_gga18_mga20_path_ref, real_gga18_mga20_path_tar,
                real_gga18_mga20_ref_name, real_gga18_mga20_tar_name,
                par_main, par_viz)


def run_real_gga14_mga16():
    par_main = '-rm 14,0,0.005,0.95/5,0,0.99,0.95 -f 200 -d 1500 -th 1.95 ' + \
        '-e 1.95 -m 400000 -dp'
    par_viz = '-l 1 -p 1 -rn "GGA 14" -tn "MGA 16" -stat -o GGA14_MGA16.svg'
    run_smashpp(real_gga14_mga16_path_ref, real_gga14_mga16_path_tar,
                real_gga14_mga16_ref_name, real_gga14_mga16_tar_name,
                par_main, par_viz)


def run_real_hs12_pt12():
    par_main = '-rm 14,0,0.001,0.95 -f 9000 -d 500 -th 1.9 -m 100000 -dp'
    par_viz = '-l 1 -p 1 -vv -rn "HS 12" -tn "PT 12" -stat -o HS12_PT12.svg'
    run_smashpp(real_hs12_pt12_path_ref, real_hs12_pt12_path_tar,
                real_hs12_pt12_ref_name, real_hs12_pt12_tar_name,
                par_main, par_viz)


if run:
    # Synthetic
    run_synth_small()
    run_synth_medium()
    run_synth_large()
    run_synth_xlarge()
    run_synth_mutate()

    # Real
    run_real_PXO99A_MAFF()
    run_real_gga18_mga20()
    run_real_gga14_mga16()
    run_real_hs12_pt12()

    # Compare with Smash. Synthetic & Real
    run_comp_smash()

if bench:
    bench_result = []  # name, size, time, memory

    # Synthetic
    bench_synth_small = False
    bench_synth_medium = False
    bench_synth_large = False
    bench_synth_xlarge = False
    bench_synth_mutate = False
    bench_comp_smash = False

    # Real
    bench_real_PXO99A_MAFF = False
    bench_real_gga18_mga20 = False
    bench_real_gga14_mga16 = False
    bench_real_hs12_pt12 = False

    if bench_synth_small:
        name = 'Small'
        cat = 'Synthetic'
        size = os.path.getsize(path_data_synth + synth_small_ref_name) + \
            os.path.getsize(path_data_synth + synth_small_tar_name)
        start_time = time.perf_counter()
        memory = memory_usage(run_synth_small)
        end_time = time.perf_counter()
        elapsed = f"{end_time - start_time:.2f}"
        max_memory = f"{max(memory):.2f}"
        bench_result.append([name, cat, size, elapsed, max_memory])

    if bench_synth_medium:
        name = 'Medium'
        cat = 'Synthetic'
        size = os.path.getsize(path_data_synth + synth_medium_ref_name) + \
            os.path.getsize(path_data_synth + synth_medium_tar_name)
        start_time = time.perf_counter()
        memory = memory_usage(run_synth_medium)
        end_time = time.perf_counter()
        elapsed = f"{end_time - start_time:.0f}"
        max_memory = f"{max(memory):.2f}"
        bench_result.append([name, cat, size, elapsed, max_memory])

    if bench_synth_large:
        name = 'Large'
        cat = 'Synthetic'
        size = os.path.getsize(path_data_synth + synth_large_ref_name) + \
            os.path.getsize(path_data_synth + synth_large_tar_name)
        start_time = time.perf_counter()
        memory = memory_usage(run_synth_large)
        end_time = time.perf_counter()
        elapsed = f"{end_time - start_time:.2f}"
        max_memory = f"{max(memory):.2f}"
        bench_result.append([name, cat, size, elapsed, max_memory])

    if bench_synth_xlarge:
        name = 'XLarge'
        cat = 'Synthetic'
        size = os.path.getsize(path_data_synth + synth_xlarge_ref_name) + \
            os.path.getsize(path_data_synth + synth_xlarge_tar_name)
        start_time = time.perf_counter()
        memory = memory_usage(run_synth_xlarge)
        end_time = time.perf_counter()
        elapsed = f"{end_time - start_time:.0f}"
        max_memory = f"{max(memory):.2f}"
        bench_result.append([name, cat, size, elapsed, max_memory])

    if bench_synth_mutate:
        name = 'Mutate'
        cat = 'Synthetic'
        size = os.path.getsize(path_data_synth + synth_mutate_ref_name) + \
            os.path.getsize(path_data_synth + synth_mutate_tar_name)
        start_time = time.perf_counter()
        memory = memory_usage(run_synth_mutate)
        end_time = time.perf_counter()
        elapsed = f"{end_time - start_time:.0f}"
        max_memory = f"{max(memory):.2f}"
        bench_result.append([name, cat, size, elapsed, max_memory])

    if bench_real_PXO99A_MAFF:
        name = 'PXO99A_MAFF'
        cat = 'Real'
        size = \
            os.path.getsize(real_PXO99A_MAFF_path_ref +
                            real_PXO99A_MAFF_ref_name) + \
            os.path.getsize(real_PXO99A_MAFF_path_tar +
                            real_PXO99A_MAFF_tar_name)
        start_time = time.perf_counter()
        memory = memory_usage(run_real_PXO99A_MAFF)
        end_time = time.perf_counter()
        elapsed = f"{end_time - start_time:.0f}"
        max_memory = f"{max(memory):.2f}"
        bench_result.append([name, cat, size, elapsed, max_memory])

    if bench_real_gga18_mga20:
        name = 'GGA18_MGA20'
        cat = 'Real'
        size = \
            os.path.getsize(real_gga18_mga20_path_ref +
                            real_gga18_mga20_ref_name) + \
            os.path.getsize(real_gga18_mga20_path_tar +
                            real_gga18_mga20_tar_name)
        start_time = time.perf_counter()
        memory = memory_usage(run_real_gga18_mga20)
        end_time = time.perf_counter()
        elapsed = f"{end_time - start_time:.0f}"
        max_memory = f"{max(memory):.2f}"
        bench_result.append([name, cat, size, elapsed, max_memory])

    if bench_real_gga14_mga16:
        name = 'GGA14_MGA16'
        cat = 'Real'
        size = \
            os.path.getsize(real_gga14_mga16_path_ref +
                            real_gga14_mga16_ref_name) + \
            os.path.getsize(real_gga14_mga16_path_tar +
                            real_gga14_mga16_tar_name)
        start_time = time.perf_counter()
        memory = memory_usage(run_real_gga14_mga16)
        end_time = time.perf_counter()
        elapsed = f"{end_time - start_time:.0f}"
        max_memory = f"{max(memory):.2f}"
        bench_result.append([name, cat, size, elapsed, max_memory])

    if bench_real_hs12_pt12:
        name = 'HS12_PT12'
        cat = 'Real'
        size = \
            os.path.getsize(real_hs12_pt12_path_ref +
                            real_hs12_pt12_ref_name) + \
            os.path.getsize(real_hs12_pt12_path_tar +
                            real_hs12_pt12_tar_name)
        start_time = time.perf_counter()
        memory = memory_usage(run_real_hs12_pt12)
        end_time = time.perf_counter()
        elapsed = f"{end_time - start_time:.0f}"
        max_memory = f"{max(memory):.2f}"
        bench_result.append([name, cat, size, elapsed, max_memory])

    # if bench_comp_smash:
    #     name = 'Comp_Smash'
    #     cat = 'Synthetic'
        # size = \
        #     os.path.getsize(path_data_synth + synth_comp_smash_ref_name) + \
        #     os.path.getsize(path_data_synth + synth_comp_smash_tar_name)
    #     start_time = time.perf_counter()
    #     memory = memory_usage(run_comp_smash)
    #     end_time = time.perf_counter()
    #     elapsed = f"{end_time - start_time:.0f}"
    #     max_memory = f"{max(memory):.2f}"
    #     bench_result.append([name, cat, size, elapsed, max_memory])

    with open('bench.csv', 'w') as bench_file:
        writer = csv.writer(bench_file)
        writer.writerow(['Name', 'Cat', 'Size.B', 'Time.s', 'Memory.MB'])
        writer.writerows(bench_result)

# if sim_permute:
#     ref = 'RefPerm'
#     tar = 'TarPerm'
#     viz_par = ' -l 6 -s 30 -w 13 -p 1 -vv '

#     # Original
#     # execute(smashpp + '-r ' + path_data_synth + ref + ' -t ' +
#     #         path_data_synth + tar + ' -th 1.8 ' +
#     #         '-rm 14,0,0.001,0.95/5,0,0.001,0.95 -f 100 -d 3000 -nr -sf ')
#     # execute(smashpp + '-viz -rn Ref -tn Tar ' + viz_par +
#     #         '-o Perm.svg ' + ref + '.' + tar + '.pos')

# #     copyfile(path_data_synth + ref, ref)
# #     copyfile(path_data_synth + tar, tar)
# #     execute(smash + ' -t 1.6 -c 14 -w 100 -d 3000 ' + ref + ' ' + tar)
# #     os.remove(ref)
# #     os.remove(tar)
# #     remove_all_ext(current_dir, 'ext')
# #     remove_all_ext(current_dir, 'rev')
# #     remove(current_dir, '*.sys*')

#     # Permutated
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

#     execute(smashpp + '-r ' + path_data_synth + ref_perm +
# #           ' -t ' + path_data_synth + tar +
# #           ' -th 5 -rm 14,0,0.001,0.95:3,0,0.001,0.95 -f 3000 -d 1 -ar ' +
# #           '-nr -sf ')
#     execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
#             '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

# #     block_size = 2000000
# #     ref_perm = ref + str(block_size)
# #     execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
# #             '-s 165604 < ' + path_data_synth + ref +
# #             ' > ' + path_data_synth + ref_perm)
# #     execute(smashpp + '-r ' + path_data_synth + ref_perm +
# #             ' -t ' + path_data_synth + tar +
# #             ' -th 1.5 -rm 14,0,0.001,0.9 -f 100 -d 10000 -ar -dp ')
# #     execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
# #             '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

# #     block_size = 10000
# #     ref_perm = ref + str(block_size)
# #     execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
# #             '-s 328914 < ' + path_data_synth + ref +
# #             ' > ' + path_data_synth + ref_perm)
# #     execute(smashpp + '-r ' + path_data_synth + ref_perm +
# #             ' -t ' + path_data_synth + tar +
# #             ' -th 1.5 -rm 14,0,0.001,0.9 -f 75 -d 1500 -ar -dp ')
# #     execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
# #             '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

# #     block_size = 40
# #     ref_perm = ref + str(block_size)
# #     execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
# #             '-s 564283 < ' + path_data_synth + ref +
# #             ' > ' + path_data_synth + ref_perm)
# #     execute(smashpp + '-r ' + path_data_synth + ref_perm +
# #             ' -t ' + path_data_synth + tar +
# #             ' -th 1.5 -rm 14,0,0.001,0.9 -f 3000 -d 1 -ar -dp ')
# #     execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
# #             '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

# # if sim_permute_smash:
# #     ref = 'RefPerm'
# #     tar = 'TarPerm'

# #     # Original
# #     copyfile(path_data_synth + ref, ref)
# #     copyfile(path_data_synth + tar, tar)
# #     execute(smash + ' -t 1.5 -c 14 -w 100 -d 10000 ' + ref + ' ' + tar)
# #     os.remove(ref)
# #     os.remove(tar)
# #     remove_all_ext(current_dir, 'ext')
# #     remove_all_ext(current_dir, 'rev')
# #     remove(current_dir, '*.sys*')

# #     # Permutated
# #     block_size = 2000000
# #     ref_perm = ref + str(block_size)
# #     copyfile(path_data_synth + ref_perm, ref_perm)
# #     copyfile(path_data_synth + tar, tar)
# #     execute(smash + ' -t 1.5 -c 14 -w 100 -d 10000 ' +
# #             ref_perm + ' ' + tar)
# #     os.remove(ref_perm)
# #     os.remove(tar)
# #     remove_all_ext(current_dir, 'ext')
# #     remove_all_ext(current_dir, 'rev')
# #     remove(current_dir, '*.sys*')

# #     block_size = 10000
# #     ref_perm = ref + str(block_size)
# #     copyfile(path_data_synth + ref_perm, ref_perm)
# #     copyfile(path_data_synth + tar, tar)
# #     execute(smash + ' -t 1.5 -c 14 -w 100 -d 10000 ' +
# #             ref_perm + ' ' + tar)
# #     os.remove(ref_perm)
# #     os.remove(tar)
# #     remove_all_ext(current_dir, 'ext')
# #     remove_all_ext(current_dir, 'rev')
# #     remove(current_dir, '*.sys*')

# #     block_size = 40
# #     ref_perm = ref + str(block_size)
# #     copyfile(path_data_synth + ref_perm, ref_perm)
# #     copyfile(path_data_synth + tar, tar)
# #     execute(smash + ' -t 1.5 -c 14 -w 100 -d 10000 ' +
# #             ref_perm + ' ' + tar)
# #     os.remove(ref_perm)
# #     os.remove(tar)
# #     remove_all_ext(current_dir, 'ext')
# #     remove_all_ext(current_dir, 'rev')
# #     remove(current_dir, '*.sys*')
