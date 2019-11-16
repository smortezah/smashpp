import os
import subprocess
import shutil
import matplotlib.pyplot as plt
import numpy as np

prepare_data = False
numerate_dataset = False
denumerate_dataset = False
find_simil_seqs = False
find_simil_regions = False
plot_rearrange_count = False
make_rearrange_mat_symmetric = False
plot_simil = False

main_file = 'mtDNA_Chordata_3327_22-03-2019.fasta'
data_in_path = 'Taxonomy_Chordata_NCBI_Diogo_06-05-2019'
# num_files = 3327
ave_ent_file = 'ent'
threshold = 0.2  # in (0, 1]
ent_threshold = 8 * threshold
group_list = ['Mammalia', 'Agnatha', 'Placodermes', 'Chondrichthyes',
              'Actinopterygii', 'Sarcopterygii']
dataset_path = 'dataset'
result_path = 'result'
geco = './GeCo '

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'


def execute(cmd):
    subprocess.call(cmd.split())
    # os.popen(cmd).read()


if prepare_data:
    if not os.path.exists(dataset_path):
        os.mkdir(dataset_path)
    for group in group_list:
        if not os.path.exists(dataset_path + sep + group.lower()):
            os.mkdir(dataset_path + sep + group.lower())

    print('Linearizing DNA reads ...', end="\r")
    cox1_pos_file = open('COX1_pos.tsv', 'w+')
    cox1_pos_file.write('Name' + '\t' + 'Group' + '\t' + 'Organism' +
                        '\t' + 'COX1_beg' + '\t' + 'COX1_end' + '\n')
    for input_file_name in os.listdir(data_in_path):
        in_file = open(data_in_path + sep + input_file_name)
        for line in in_file:
            if line[2:10].lower() == 'sequence':
                sequence = line[15:-3]
            if line[2:10].lower() == 'organism':
                organism = line[15:-3]
            if line[2:10].lower() == 'taxonomy':
                taxonomy = line[15:-3].lower().split('; ')
                group = ''
                for item in group_list:
                    if item.lower() in taxonomy:
                        group = item
                        break
            if line[2:6].lower() == 'cox1':
                line_separated = line.split(', ')
                cox1_beg = line_separated[1][1:-1]
                cox1_end = line_separated[2][1:-3]
                break

        seq_file = open(dataset_path + sep + group.lower() +
                        sep + input_file_name + '.seq', 'w')
        seq_file.write(sequence[int(cox1_beg)-1:].upper())
        seq_file.write(sequence[0:int(cox1_beg)-1].upper() + '\n')
        seq_file.close()

        cox1_pos_file.write(input_file_name + '\t' + group + '\t' + organism +
                            '\t' + cox1_beg + '\t' + cox1_end + '\n')
    print('Linearizing DNA reads finished.')

    # # Split reads
    # cmd = './goose-splitreads < ' + main_file
    # os.popen(cmd).read()

    # # Convert fasta to seq
    # for file in os.listdir("."):
    #     if file.endswith("fasta"):
    #         cmd = './goose-fasta2seq < ' + file + ' > ' + file[3:-6]
    #         os.popen(cmd).read()
    #         if os.path.exists(file):
    #             os.remove(file)


def numerate(Class):
    data_path = dataset_path + sep + Class.lower() + sep
    idx = 1
    for file in os.listdir(data_path):
        if idx < 10:
            pre = '000'
        elif idx < 100:
            pre = '00'
        elif idx < 1000:
            pre = '0'
        elif idx < 10000:
            pre = ''

        os.rename(data_path + file, data_path +
                  pre + str(idx) + '_' + file)
        idx += 1


def denumerate(Class):
    data_path = dataset_path + sep + Class.lower() + sep
    for file in os.listdir(data_path):
        os.rename(data_path + file, data_path + file[5:])


if numerate_dataset:
    numerate('Chondrichthyes')
    numerate('Mammalia')
    numerate('Actinopterygii')

if denumerate_dataset:
    denumerate('Chondrichthyes')
    denumerate('Mammalia')
    denumerate('Actinopterygii')

if find_simil_seqs:
    # Actinopterygii
    in_file_path = dataset_path + sep + 'actinopterygii'
    in_file_list = os.listdir(in_file_path)

    out_file_name = ave_ent_file + '_Actinopterygii.tsv'
    if os.path.exists(out_file_name):
        os.remove(out_file_name)
    out_file = open(out_file_name, "w")
    for file in in_file_list:
        out_file.write('\t' + file[:-4])
    out_file.write('\n')

    first = 0
    for i in range(first, len(in_file_list)):
        out_file.write(in_file_list[i][:-4])
        for j in range(first, len(in_file_list)):
            ref = in_file_path + sep + in_file_list[i]
            tar = in_file_path + sep + in_file_list[j]
            execute(geco + '-rm 6:1:0:0/0 -rm 10:10:1:0/0 -rm 14:50:1:3/10 ' +
                    '-c 30 -g 0.95 -r ' + ref + ' ' + tar + ' > log')

            with open('log', 'r') as log_file:
                for line in log_file:
                    line_list = line.split()
                    if len(line_list) > 5:
                        out_file.write('\t' + str(line_list[5]))
        out_file.write('\n')

    for file in ["log", "*.co"]:
        if os.path.exists(file):
            os.remove(file)

    # # Mammalia
    # in_file_path = dataset_path + sep + 'mammalia'
    # in_file_list = os.listdir(in_file_path)

    # out_file_name = ave_ent_file + '_Mammalia.tsv'
    # if os.path.exists(out_file_name):
    #     os.remove(out_file_name)
    # out_file = open(out_file_name, "w")
    # for file in in_file_list:
    #     out_file.write('\t' + file[:-4])
    # out_file.write('\n')

    # first = 0
    # for i in range(first, len(in_file_list)):
    #     out_file.write(in_file_list[i][:-4])
    #     for j in range(first, len(in_file_list)):
    #         ref = in_file_path + sep + in_file_list[i]
    #         tar = in_file_path + sep + in_file_list[j]
    #         execute(geco + '-rm 6:1:0:0/0 -rm 10:10:1:0/0 -rm 14:50:1:3/10 ' +
    #                 '-c 30 -g 0.95 -r ' + ref + ' ' + tar + ' > log')

    #         with open('log', 'r') as log_file:
    #             for line in log_file:
    #                 line_list = line.split()
    #                 if len(line_list) > 5:
    #                     out_file.write('\t' + str(line_list[5]))
    #     out_file.write('\n')

    # for file in ["log", "*.co"]:
    #     if os.path.exists(file):
    #         os.remove(file)

    ## Chondrichthyes
    # in_file_path = dataset_path + sep + 'chondrichthyes'
    # in_file_list = os.listdir(in_file_path)

    # out_file_name = ave_ent_file + '_Chondrichthyes.tsv'
    # if os.path.exists(out_file_name):
    #     os.remove(out_file_name)
    # out_file = open(out_file_name, "w")
    # for file in in_file_list:
    #     out_file.write('\t' + file[:-4])
    # out_file.write('\n')

    # first = 0
    # for i in range(first, len(in_file_list)):
    #     out_file.write(in_file_list[i][:-4])
    #     for j in range(first, len(in_file_list)):
    #         ref = in_file_path + sep + in_file_list[i]
    #         tar = in_file_path + sep + in_file_list[j]
    #         execute(geco + '-rm 6:1:0:0/0 -rm 10:10:1:0/0 -rm 14:50:1:3/10 ' +
    #                 '-c 30 -g 0.95 -r ' + ref + ' ' + tar + ' > log')

    #         with open('log', 'r') as log_file:
    #             for line in log_file:
    #                 line_list = line.split()
    #                 if len(line_list) > 5:
    #                     out_file.write('\t' + str(line_list[5]))
    #     out_file.write('\n')

    # for file in ["log", "*.co"]:
    #     if os.path.exists(file):
    #         os.remove(file)

    out_file.close()


def build_simil_matrix(nrc_mat, threshold):
    num_files = len(nrc_mat)
    simil_mat = [[0 for x in range(num_files)] for y in range(num_files)]
    for i in range(0, num_files):
        for j in range(i+1, num_files):
            if min(nrc_mat[i][j], nrc_mat[j][i]) < threshold:
                simil_mat[i][j] = 1
    return simil_mat


def count_rearrange(file_name):
    with open(file_name) as file:
        for i, l in enumerate(file, -3):  # 4 lines of headers
            pass
        return i


def apply_smashpp(Class):
    geco_threshold = 1.5
    nrc_file = open(ave_ent_file + '_' + Class + '.tsv')
    data_path = dataset_path + sep + Class.lower() + sep
    exe_param = '-rm 11,0,0.001,0.95/8,0,0.01,0.9 -m 20 -f 100 -th 1.8 '
    # exe_param = '-rm 11,0,0.001,0.95/8,0,0.01,0.9 -m 20 -f 100 -dp -th 1.8 '

    header = nrc_file.readline().split()
    nrc_file.seek(0)
    nrc_mat = np.genfromtxt(nrc_file, skip_header=True,
                            usecols=range(1, len(header) + 1))
    simil_mat = build_simil_matrix(nrc_mat, geco_threshold)

    if os.name == 'posix':
        smashpp_bin = '../../smashpp '
    elif os.name == 'nt':
        smashpp_bin = '..\..\smashpp.exe '

    if not os.path.exists(result_path):
        os.mkdir(result_path)

    rearrange_count_name = 'rearrange_count_' + Class + '.tsv'
    if os.path.exists(rearrange_count_name):
        os.remove(rearrange_count_name)
    with open(rearrange_count_name, 'w') as rearrange_count:
        for i in range(len(header)):
            rearrange_count.write('\t' + header[i])
        rearrange_count.write('\n')

        for i, nameI in enumerate(header):
            rearrange_count.write(nameI)
            for j, nameJ in enumerate(header):
                if simil_mat[i][j] == 0:
                    rearrange_count.write('\t0')
                elif simil_mat[i][j] == 1:
                    execute(smashpp_bin + exe_param +
                            '-r ' + data_path + nameI + '.seq ' +
                            '-t ' + data_path + nameJ + '.seq ')

                    pos_file_name = nameI + '.seq.' + nameJ + '.seq.pos'
                    rearrange_count.write(
                        '\t' + str(count_rearrange(pos_file_name)))

                    if os.path.exists(result_path + sep + pos_file_name):
                        os.remove(result_path + sep + pos_file_name)
                    shutil.move(pos_file_name, result_path)
            rearrange_count.write('\n')


if find_simil_regions:
    # Class = 'Chondrichthyes'
    Class = 'Mammalia'
    # Class = 'Actinopterygii'

    print('Finding similar regions in ' + Class)
    apply_smashpp(Class)
    print('Finished.')


if make_rearrange_mat_symmetric:
    # Class = 'Chondrichthyes'
    # Class = 'Mammalia'
    Class = 'Actinopterygii'
    in_file = open('rearrange_count_' + Class + '.tsv')
    out_file_name = 'rearrange_count_' + Class + '_symmetric.tsv'

    header = in_file.readline().split()
    in_file.seek(0)
    rearrange_count = np.genfromtxt(in_file, skip_header=True, dtype=np.int64,
                                    usecols=range(1, len(header) + 1))
    for i, valI in enumerate(rearrange_count):
        for j, valJ in enumerate(rearrange_count):
            if i > j:
                rearrange_count[i][j] = rearrange_count[j][i]

    if os.path.exists(out_file_name):
        os.remove(out_file_name)
    with open(out_file_name, 'w') as rearrange_count_symmetric:
        for i, nameI in enumerate(header):
            rearrange_count_symmetric.write('\t' + nameI)
        rearrange_count_symmetric.write('\n')
        for i, nameI in enumerate(header):
            rearrange_count_symmetric.write(nameI)
            for j, nameJ in enumerate(header):
                rearrange_count_symmetric.write(
                    '\t' + str(rearrange_count[i][j]))
            rearrange_count_symmetric.write('\n')

if plot_simil:
    nrc_mat = np.genfromtxt(
        ave_ent_file + '_Chondrichthyes.tsv', skip_header=True)

    # for x in nrc_mat:
    #     print(*x, sep="\t")

    # simil_mat = build_simil_matrix(nrc_mat, ent_threshold)
    # # for x in simil_mat:
    # #     print(*x, sep=" ")

    # plt.matshow(nrc_mat)
    # # plt.colorbar()
    # # # plt.show()
    # # plt.savefig('nrc_mat_'+str(threshold)+'.pdf')

    # plt.matshow(simil_mat)
    # plt.colorbar()
    # plt.show()
    # plt.savefig('simil.'+str(threshold)+'.pdf')

    def plot_subplot(threshold, axis):
        axis.matshow(build_simil_matrix(nrc_mat, threshold))
        axis.set_title("Thresh = " + str(threshold/8))
        axis.xaxis.set_ticks_position('bottom')

    n_row = 2
    n_col = 3
    thresh = [[0.2, 0.5, 0.8], [1.1, 1.4, 1.7]]
    fig, axes = plt.subplots(nrows=n_row, ncols=n_col)
    plt.subplots_adjust(wspace=0.5, hspace=0.5)
    for r in range(n_row):
        for c in range(n_col):
            plot_subplot(thresh[r][c], axes[r, c])
    # plt.show()
    plt.savefig('similarity_Chondrichthyes.pdf')

if plot_rearrange_count:
    Class = 'Mammalia'  # 'Chondrichthyes'  'Actinopterygii'
    rearrange_count_mat = np.genfromtxt(
        'rearrange_count_' + Class + '.tsv', skip_header=True)
    plt.matshow(rearrange_count_mat)
    plt.colorbar()
