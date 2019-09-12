import os
from shutil import copyfile
from pathlib import Path
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
synth_permute = True
synth_compare_smash = False

# Run on simulated dataset
sim_small = False
sim_medium = False
sim_large = False
sim_xlarge = False
sim_mutation = False
sim_permute = False
real_permute = False
sim_permute_smash = False
sim_compare_smash = True

# Run on real dataset
X_oryzae_pv_oryzae_PXO99A_MAFF_311018 = False
gga18_mga20 = False
gga14_mga16 = False

hs21_gg21 = False
S_cerevisiae_IV_C_glabrata_K = False
E_gossypii_I_S_cerevisiae_XVI = False
S_cerevisiae_VIII_C_glabrata_XVI = False
K_lactis_E_gossypii = False
K_lactis_F_E_gossypii_VI = False
S_cerevisiae_5_C_glabrata_I = False
S_cerevisiae_14_C_glabrata_J = False
e_coli_s_dysenteriae = False

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
goose_permuteseqbyblocks = path_bin + 'goose-permuteseqbyblocks '
smashpp = '..' + sep + 'smashpp '
smashpp_inv_rep = path_bin + 'smashpp-inv-rep '
smash = path_bin + 'smash '
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


def remove_all_ext(directory, extension):
    for item in os.listdir(directory):
        if item.endswith(extension):
            os.remove(os.path.join(directory, item))


def remove(dir, pattern):
    for p in Path(dir).glob(pattern):
        p.unlink()


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


synth_permute = False

if synth_permute:  # sizes: ref:15,000,000, tar:15,000,000
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 15801  r_a')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.2,0.2,0.3,0.3,0.0 -ls 100 -n 10000 -s 190  r_b')
    execute(goose_fastqsimulation + synth_common_par +
            '-f 0.3,0.3,0.2,0.2,0.0 -ls 100 -n 10000 -s 8642  r_c')
    cat(['r_a', 'r_b', 'r_c'], path_data_sim + 'RefPerm')

#     execute(goose_mutatedna + '-mr 0.03 < r_a > t_c')
    copyfile('r_a', 't_c')
    execute(smashpp_inv_rep + 'r_b t_b')
    copyfile('r_c', 't_a')
    cat(['t_a', 't_b', 't_c'], path_data_sim + 'TarPerm')

if synth_compare_smash:
    if os.path.exists(path_data_sim + "RefMut_smash"):
        os.remove(path_data_sim + "RefMut_smash")
    if os.path.exists(path_data_sim + "TarMut_smash"):
        os.remove(path_data_sim + "TarMut_smash")

    for i in range(0, 9 + 1):
        execute(goose_fastqsimulation + synth_common_par +
            ' -s ' + str(i * 10 + 1) +
            '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 1000 r_' + str(i))
        append('r_' + str(i), path_data_sim + 'RefMut_smash')
    copyfile('r_0', 't_0')
    for i in range(1, 9 + 1):
        execute(goose_mutatedna + '-mr ' + str(i/100) +
                ' < r_' + str(i) + ' > t_' + str(i))
    for i in range(4, 6 + 1):
        execute(smashpp_inv_rep + 't_' + str(i) + ' t_' + str(i) + 'i')

    for i in range(0, 3 + 1):
        append('t_' + str(i), path_data_sim + 'TarMut_smash')
    for i in range(4, 6 + 1):
        append('t_' + str(i) + 'i', path_data_sim + 'TarMut_smash')
    for i in range(7, 9 + 1):
        append('t_' + str(i), path_data_sim + 'TarMut_smash')

for file in os.listdir(current_dir):
    if file.startswith("r_"):
        os.remove(os.path.join(current_dir, file))
    if file.startswith("t_"):
        os.remove(os.path.join(current_dir, file))

if sim_small:
    ref = 'RefS'
    tar = 'TarS'
    out = 'S.svg'
    execute(smashpp + '-r ' + path_data_sim + ref +
            ' -t ' + path_data_sim + tar + ' -l 3 -f 25')
    execute(smashpp + '-viz -p 1 -rt 150 -tt 150 ' +
            '-o ' + out + sim_common_par + ref + '.' + tar + '.pos')

if sim_medium:
    ref = 'RefM'
    tar = 'TarM'
    out = 'M.svg'
    execute(smashpp + '-r ' + path_data_sim +
            ref + ' -t ' + path_data_sim + tar + ' -l 3 -f 100')
    execute(smashpp + '-viz -p 1 -o ' + out +
            sim_common_par + ref + '.' + tar + '.pos')

if sim_large:
    ref = 'RefL'
    tar = 'TarL'
    out = 'L.svg'
    execute(smashpp + '-r ' + path_data_sim +
            ref + ' -t ' + path_data_sim + tar + ' -l 3 -f 135')
    execute(smashpp + '-viz -p 1 -o ' + out + sim_common_par +
            ref + '.' + tar + '.pos')

if sim_xlarge:
    ref = 'RefXL'
    tar = 'TarXL'
    out = 'XL.svg'
    execute(smashpp + '-r ' + path_data_sim +
            ref + ' -t ' + path_data_sim + tar + ' -l 3 -f 275')
    execute(smashpp + '-viz -p 1 -o ' + out + sim_common_par +
            ref + '.' + tar + '.pos')

if sim_mutation:
    ref = 'RefMut'
    tar = 'TarMut'
    out = 'Mut.svg'
    execute(smashpp + '-r ' + path_data_sim + ref + ' -t ' +
            path_data_sim + tar + ' -th 2 -rm 16,0,0.2,0.9 -f 25000 -m 15000')
    execute(smashpp + '-viz -p 1 -rt 5000 -tt 5000' +
            sim_common_par + '-o ' + out + ' ' + ref + '.' + tar + '.pos')


# sim_permute = False

if sim_permute:
    ref = 'RefPerm'
    tar = 'TarPerm'
    viz_par = ' -l 6 -s 30 -w 13 -p 1 -vv '

    # Original
#     execute(smashpp + '-r ' + path_data_sim + ref + ' -t ' +
#             path_data_sim + tar + ' -th 1.8 -rm 14,0,0.001,0.95/5,0,0.001,0.95 -f 100 -d 3000 -nr -sf ')
#     execute(smashpp + '-viz -rn Ref -tn Tar ' + viz_par +
#             '-o Perm.svg ' + ref + '.' + tar + '.pos')

#     copyfile(path_data_sim + ref, ref)
#     copyfile(path_data_sim + tar, tar)
#     execute(smash + ' -t 1.6 -c 14 -w 100 -d 3000 ' + ref + ' ' + tar)
#     os.remove(ref)
#     os.remove(tar)
#     remove_all_ext(current_dir, 'ext')
#     remove_all_ext(current_dir, 'rev')
#     remove(current_dir, '*.sys*')

    # Permutated
    block_size = 20
    ref_perm = ref + str(block_size)
#     execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
#             '-s 165604 < ' + path_data_sim + ref + ' > ' + path_data_sim + ref_perm)

#     copyfile(path_data_sim + ref_perm, ref_perm)
#     copyfile(path_data_sim + tar, tar)
#     execute(smash + ' -t 1.9 -c 14 -w 100 -d 1 ' + ref_perm + ' ' + tar)
#     os.remove(ref_perm)
#     os.remove(tar)
#     remove_all_ext(current_dir, 'ext')
#     remove_all_ext(current_dir, 'rev')
#     remove(current_dir, '*.sys*')

    execute(smashpp + '-r ' + path_data_sim + ref_perm + ' -t ' + path_data_sim +
            tar + ' -th 5 -rm 14,0,0.001,0.95:3,0,0.001,0.95 -f 3000 -d 1 -ar -nr -sf ')
    execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
            '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

#     block_size = 2000000
#     ref_perm = ref + str(block_size)
#     execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
#             '-s 165604 < ' + path_data_sim + ref + ' > ' + path_data_sim + ref_perm)
#     execute(smashpp + '-r ' + path_data_sim + ref_perm + ' -t ' + path_data_sim +
#             tar + ' -th 1.5 -rm 14,0,0.001,0.9 -f 100 -d 10000 -ar -dp ')
#     execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
#             '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

#     block_size = 200000
#     ref_perm = ref + str(block_size)
#     execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
#             '-s 972652 < ' + path_data_sim + ref + ' > ' + path_data_sim + ref_perm)
#     execute(smashpp + '-r ' + path_data_sim + ref_perm + ' -t ' + path_data_sim +
#             tar + ' -th 1.5 -rm 14,0,0.001,0.9 -f 40 -d 8000 -ar -dp ')
#     execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
#             '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

#     block_size = 10000
#     ref_perm = ref + str(block_size)
#     execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
#             '-s 328914 < ' + path_data_sim + ref + ' > ' + path_data_sim + ref_perm)
#     execute(smashpp + '-r ' + path_data_sim + ref_perm + ' -t ' +
#             path_data_sim + tar + ' -th 1.5 -rm 14,0,0.001,0.9 -f 75 -d 1500 -ar -dp ')
#     execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par + '-o ' + ref_perm +
#             '.svg ' + ref_perm + '.' + tar + '.pos')

#     block_size = 40
#     ref_perm = ref + str(block_size)
#     execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
#             '-s 564283 < ' + path_data_sim + ref + ' > ' + path_data_sim + ref_perm)
#     execute(smashpp + '-r ' + path_data_sim + ref_perm + ' -t ' + path_data_sim +
#             tar + ' -th 1.5 -rm 14,0,0.001,0.9 -f 3000 -d 1 -ar -dp ')
#     execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
#             '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

if real_permute:
    path_ref = path_data_real + 'mammalia' + sep + 'Homo_sapiens' + sep
    path_tar = path_data_real + 'mammalia' + sep + 'Pan_paniscus' + sep
    ref_main = '19.fa'
    ref = 'hs' + ref_main
    tar_main = '19.seq'
    tar = 'pp' + tar_main
    viz_par = ' -l 6 -s 30 -w 13 -p 1 -vv '

    # Original
    # execute(smashpp + '-r ' + path_ref + ref + ' -t ' + path_tar +
    #         tar + ' -th 1.3 -rm 20,0,0.001,0.9 -f 100 -d 33000 -nr -sf ')
    # execute(smashpp + '-viz -rn Ref -tn Tar ' + viz_par +
    #         '-o Perm_real.svg ' + ref + '.' + tar + '.pos')

    copyfile(path_ref + ref_main, ref)
    copyfile(path_tar + tar_main, tar)
    execute(smash + ' -t 1.5 -c 14 -w 100 -d 10000 ' + ref + ' ' + tar)
    os.remove(ref)
    os.remove(tar)
    remove_all_ext(current_dir, 'ext')
    remove_all_ext(current_dir, 'rev')
    remove(current_dir, '*.sys*')

    # Permutated
    block_size = 2000000
    ref_perm = ref + str(block_size)
    execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
            '-s 165604 < ' + path_data_sim + ref + ' > ' + path_data_sim + ref_perm)
    execute(smashpp + '-r ' + path_data_sim + ref_perm + ' -t ' + path_data_sim +
            tar + ' -th 1.5 -rm 14,0,0.001,0.9 -f 100 -d 10000 -ar -dp ')
    execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
            '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

    block_size = 200000
    ref_perm = ref + str(block_size)
    execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
            '-s 972652 < ' + path_data_sim + ref + ' > ' + path_data_sim + ref_perm)
    execute(smashpp + '-r ' + path_data_sim + ref_perm + ' -t ' + path_data_sim +
            tar + ' -th 1.5 -rm 14,0,0.001,0.9 -f 40 -d 8000 -ar -dp ')
    execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
            '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

    block_size = 10000
    ref_perm = ref + str(block_size)
    execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
            '-s 328914 < ' + path_data_sim + ref + ' > ' + path_data_sim + ref_perm)
    execute(smashpp + '-r ' + path_data_sim + ref_perm + ' -t ' +
            path_data_sim + tar + ' -th 1.5 -rm 14,0,0.001,0.9 -f 75 -d 1500        -ar -dp ')
    execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par + '-o ' + ref_perm +
            '.svg ' + ref_perm + '.' + tar + '.pos')

    block_size = 40
    ref_perm = ref + str(block_size)
    execute(goose_permuteseqbyblocks + '-bs ' + str(block_size) +
            '-s 564283 < ' + path_data_sim + ref + ' > ' + path_data_sim + ref_perm)
    execute(smashpp + '-r ' + path_data_sim + ref_perm + ' -t ' + path_data_sim +
            tar + ' -th 1.5 -rm 14,0,0.001,0.9 -f 3000 -d 1 -ar -dp ')
    execute(smashpp + '-viz -rn Ref_perm -tn Tar ' + viz_par +
            '-o ' + ref_perm + '.svg ' + ref_perm + '.' + tar + '.pos')

if sim_permute_smash:
    ref = 'RefPerm'
    tar = 'TarPerm'

    # Original
    copyfile(path_data_sim + ref, ref)
    copyfile(path_data_sim + tar, tar)
    execute(smash + ' -t 1.5 -c 14 -w 100 -d 10000 ' + ref + ' ' + tar)
    os.remove(ref)
    os.remove(tar)
    remove_all_ext(current_dir, 'ext')
    remove_all_ext(current_dir, 'rev')
    remove(current_dir, '*.sys*')

    # Permutated
    block_size = 2000000
    ref_perm = ref + str(block_size)
    copyfile(path_data_sim + ref_perm, ref_perm)
    copyfile(path_data_sim + tar, tar)
    execute(smash + ' -t 1.5 -c 14 -w 100 -d 10000 ' + ref_perm + ' ' + tar)
    os.remove(ref_perm)
    os.remove(tar)
    remove_all_ext(current_dir, 'ext')
    remove_all_ext(current_dir, 'rev')
    remove(current_dir, '*.sys*')

    block_size = 200000
    ref_perm = ref + str(block_size)
    copyfile(path_data_sim + ref_perm, ref_perm)
    copyfile(path_data_sim + tar, tar)
    execute(smash + ' -t 1.5 -c 14 -w 100 -d 10000 ' + ref_perm + ' ' + tar)
    os.remove(ref_perm)
    os.remove(tar)
    remove_all_ext(current_dir, 'ext')
    remove_all_ext(current_dir, 'rev')
    remove(current_dir, '*.sys*')

    block_size = 10000
    ref_perm = ref + str(block_size)
    copyfile(path_data_sim + ref_perm, ref_perm)
    copyfile(path_data_sim + tar, tar)
    execute(smash + ' -t 1.5 -c 14 -w 100 -d 10000 ' + ref_perm + ' ' + tar)
    os.remove(ref_perm)
    os.remove(tar)
    remove_all_ext(current_dir, 'ext')
    remove_all_ext(current_dir, 'rev')
    remove(current_dir, '*.sys*')

    block_size = 40
    ref_perm = ref + str(block_size)
    copyfile(path_data_sim + ref_perm, ref_perm)
    copyfile(path_data_sim + tar, tar)
    execute(smash + ' -t 1.5 -c 14 -w 100 -d 10000 ' + ref_perm + ' ' + tar)
    os.remove(ref_perm)
    os.remove(tar)
    remove_all_ext(current_dir, 'ext')
    remove_all_ext(current_dir, 'rev')
    remove(current_dir, '*.sys*')

if sim_compare_smash:
    ref = 'RefMut_smash'
    tar = 'TarMut_smash'
    viz_par = ' -l 6 -s 30 -w 13 -p 1 -vv '

    # # Smash++
    # execute(smashpp + '-r ' + path_data_sim + ref + ' -t ' +
    #         path_data_sim + tar + ' -th 1.6 -rm 14,0,0.001,0.95/5,0,0.001,0.95 -f 1000 -d 50 -nr -sf ')
    # execute(smashpp + '-viz -rn Ref -tn Tar -rt 100000 -tt 100000 ' + viz_par +
    #         '-o Mut_smash.svg ' + ref + '.' + tar + '.pos')

    # Smash
    copyfile(path_data_sim + ref, ref)
    copyfile(path_data_sim + tar, tar)
    execute(smash + ' -t 1.75 -c 14 -d 1 -w 5000 -m 1000 ' + ref + ' ' + tar)
    os.remove(ref)
    os.remove(tar)
    remove_all_ext(current_dir, 'ext')
    remove_all_ext(current_dir, 'rev')
    remove(current_dir, '*.sys*')

if X_oryzae_pv_oryzae_PXO99A_MAFF_311018:
    path = path_data_real + 'bacteria' + sep + 'Xanthomonas_oryzae_pv_oryzae' + sep
    ref = 'PXO99A.seq'
    tar = 'MAFF_311018.seq'
    main_par = ' -rm 13,0,0.005,1 -f 150 -m 10000 -d 1000 -th 1.55 -ar -dp '
    viz_par = ' -viz -l 6 -s 10 -w 8 -p 1 -rt 500000 -rn PXO99A -tn "MAFF 311018" -o PXO99A_MAFF_311018.svg '
    execute(smashpp + main_par + ' -r ' + path + ref + ' -t ' +
            path + tar)
    execute(smashpp + viz_par + ref + '.' + tar + '.pos')

if gga18_mga20:
    path_ref = path_data_real + 'bird' + sep + 'Gallus_gallus' + sep
    path_tar = path_data_real + 'bird' + sep + 'Meleagris_gallopavo' + sep
    ref = '18.seq'
    tar = '20.seq'
    main_par = ' -rm 14,0,0.005,0.95/5,0,1,0.95 -f 130 -m 500000 -d 2200 -th 1.9 -dp '
    viz_par = ' -viz -l 1 -p 1 -vv -tc 6 -rn "GGA 18" -tn "MGA 20" -o GGA18_MGA20.svg '
    execute(smashpp + main_par + ' -r ' + path_ref + ref + ' -t ' +
            path_tar + tar)
    execute(smashpp + viz_par + ref + '.' + tar + '.pos')

if gga14_mga16:
    path_ref = path_data_real + 'bird' + sep + 'Gallus_gallus' + sep
    path_tar = path_data_real + 'bird' + sep + 'Meleagris_gallopavo' + sep
    ref = '14.seq'
    tar = '16.seq'
    main_par = ' -rm 14,0,0.005,0.95/5,0,0.99,0.95 -f 200 -d 1500 -th 1.95 -e 1.95 -m 400000 -dp '
    viz_par = ' -viz -l 1 -p 1 -rn "GGA 14" -tn "MGA 16" -o GGA14_MGA16.svg '
    execute(smashpp + main_par + ' -r ' + path_ref + ref + ' -t ' +
            path_tar + tar)
    execute(smashpp + viz_par + ref + '.' + tar + '.pos')

if hs21_gg21:
    path_ref = path_data_real + 'mammalia' + sep + 'Homo_sapiens' + sep
    path_tar = path_data_real + 'mammalia' + sep + 'Gorilla_gorilla_gorilla' + sep
    ref = '21.seq'
    tar = '21.seq'
    main_par = ' -rm 18,0,0.001,0.95/5,0,0.9,0.95 -f 150 -d 12000 -th 0.1 -e 2 -m 1000000 -nr -sf '
    viz_par = ' -viz -l 1 '
    execute(smashpp + main_par + ' -r ' + path_ref + ref + ' -t ' +
            path_tar + tar)
    execute(smashpp + viz_par + ref + '.' + tar + '.pos')

if E_gossypii_I_S_cerevisiae_XVI:
    path_tar = path_data_real + 'fungi' + sep + 'Eremothecium_gossypii' + sep
    path_ref = path_data_real + 'fungi' + sep + 'Saccharomyces_cerevisiae' + sep
    tar = 'I.seq'
    ref = 'XVI.seq'
    # out = 'S_cerevisiae_C_glabrata.svg'
    execute(smashpp + '-r ' + path_ref + ref + ' -t ' +
            path_tar + tar + '  -l 0 -ar -f 200  -nr ')
    execute(smashpp + '-viz ' + ref + '.' + tar + '.pos')

if S_cerevisiae_VIII_C_glabrata_XVI:
    path_ref = path_data_real + 'fungi' + sep + 'Saccharomyces_cerevisiae' + sep
    path_tar = path_data_real + 'fungi' + sep + 'Saccharomyces_cerevisiae' + sep
    ref = 'VIII.seq'
    tar = 'XVI.seq'
    # out = 'S_cerevisiae_C_glabrata.svg'
    execute(smashpp + '-r ' + path_ref + ref + ' -t ' +
            path_tar + tar + '  -l 3 -f 500  -nr ')
    execute(smashpp + '-viz ' + ref + '.' + tar + '.pos')

if S_cerevisiae_IV_C_glabrata_K:
    path_ref = path_data_real + 'fungi' + sep + 'Saccharomyces_cerevisiae' + sep
    path_tar = path_data_real + 'fungi' + sep + 'Candida_glabrata' + sep
    ref = 'IV.seq'
    tar = 'K.seq'
    main_par = ' -rm 14,0,0.005,0.95/5,0,0.99,0.95 -f 200 -d 275 -th 1.98 -e 2 -m 5000 -nr -sf '
    viz_par = ' -viz -l 1 -o S_cerevisiae_IV_C_glabrata_K.svg '
    execute(smashpp + main_par + ' -r ' + path_ref + ref + ' -t ' +
            path_tar + tar)
    execute(smashpp + viz_par + ref + '.' + tar + '.pos')

if K_lactis_E_gossypii:
    path_ref = path_data_real + 'fungi' + sep + 'Kluyveromyces_lactis' + sep
    path_tar = path_data_real + 'fungi' + sep + 'Eremothecium_gossypii' + sep
    ref = 'C.seq'
    tar = 'VI.seq'
    out = 'K_lactis_E_gossypii.svg'
    execute(smashpp + '-r ' + path_ref + ref + ' -t ' +
            path_tar + tar + '  -l 5 -f 150  -nr -v')
    execute(smashpp + '-viz ' + ref + '.' + tar + '.pos')

if K_lactis_F_E_gossypii_VI:
    path_ref = path_data_real + 'fungi' + sep + 'Kluyveromyces_lactis' + sep
    path_tar = path_data_real + 'fungi' + sep + 'Eremothecium_gossypii' + sep
    ref = 'F.seq'
    tar = 'VI.seq'
    out = 'K_lactis_E_gossypii.svg'
    execute(smashpp + '-r ' + path_ref + ref + ' -t ' +
            path_tar + tar + '  -l 3 -f 150  -nr -v')
    execute(smashpp + '-viz ' + ref + '.' + tar + '.pos')

if S_cerevisiae_5_C_glabrata_I:
    path_tar = path_data_real + 'fungi' + sep + 'Saccharomyces_cerevisiae' + sep
    path_ref = path_data_real + 'fungi' + sep + 'Candida_glabrata' + sep
    tar = '5.seq'
    ref = 'I.seq'
    out = 'S_cerevisiae_C_glabrata.svg'
    execute(smashpp + '-r ' + path_ref + ref + ' -t ' +
            path_tar + tar + '  -l 5 -f 50  -nr -v')
    execute(smashpp + '-viz ' + ref + '.' + tar + '.pos')

if S_cerevisiae_14_C_glabrata_J:
    path_ref = path_data_real + 'fungi' + sep + 'Saccharomyces_cerevisiae' + sep
    path_tar = path_data_real + 'fungi' + sep + 'Candida_glabrata' + sep
    ref = '14.seq'
    tar = 'J.seq'
    out = 'S_cerevisiae_C_glabrata.svg'
    execute(smashpp + '-r ' + path_ref + ref + ' -t ' +
            path_tar + tar + '  -l 5 -f 50  -nr -v')
    execute(smashpp + '-viz ' + ref + '.' + tar + '.pos')

if e_coli_s_dysenteriae:
    path = path_data_real + 'bacteria' + sep
    ref = 'e_coli_O104_H4.seq'
    tar = 's_dysenteriae_chr.seq'
    out = 'e_coli_s_dysenteriae.svg'
    execute(smashpp + '-r ' + path + ref + ' -t ' + path + tar +
            ' -th 1.8 -l 3 -f 275 -m 7500')
    # execute(smashpp + '-viz -p 1 -w 15 -s 60 -vv -o ' + out + ' ' +
    #         ref + '.' + tar + '.pos')
