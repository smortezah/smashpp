#%%
import os
import shutil
import matplotlib.pyplot as plt
import numpy as np

prepare_data = False
find_simil_seqs = False
find_simil_regions = False
plot_simil = True

main_file = 'mtDNA_Chordata_3327_22-03-2019.fasta'
data_in_path = 'Taxonomy_Chordata_NCBI_Diogo_06-05-2019'
# num_files = 3327
ave_ent_file = 'ent'
threshold = 0.2  # in (0, 1]
ent_threshold = 8 * threshold
group_list = ['Mammalia', 'Agnatha', 'Placodermes', 'Chondrichthyes',
              'Actinopterygii', 'Sarcopterygii']
dataset_path = 'dataset'
geco = './GeCo '

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'


def execute(cmd):
    os.popen(cmd).read()


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

if find_simil_seqs:
    in_file_path = dataset_path + sep + 'chondrichthyes'
    in_file_list = os.listdir(in_file_path)

    out_file_name = ave_ent_file + '_Chondrichthyes.tsv'
    if os.path.exists(out_file_name):
        os.remove(out_file_name)
    out_file = open(out_file_name, "w")
    for file in in_file_list:
        out_file.write('\t' + file[:-4])
    out_file.write('\n')

    first = 0
    for i in range(first, len(in_file_list)):
        out_file.write(in_file_list[i][:-4] + '\t')
        for j in range(first, len(in_file_list)):
            ref = in_file_path + sep + in_file_list[i]
            tar = in_file_path + sep + in_file_list[j]
            execute(geco + '-rm 6:1:0:0/0 -rm 10:10:1:0/0 -rm 14:50:1:3/10 ' +
                    '-c 30 -g 0.95 -r ' + ref + ' ' + tar + ' > log')

            with open('log', 'r') as log_file:
                for line in log_file:
                    line_list = line.split()
                    if len(line_list) > 5:
                        out_file.write(str(line_list[5]) + '\t')
        out_file.write('\n')

    for file in ["log", "*.co"]:
        if os.path.exists(file):
            os.remove(file)


def build_simil_matrix(nrc_mat):
    # num = 0
    num_files = len(nrc_mat)
    simil_mat = [[0 for x in range(num_files)] for y in range(num_files)]
    for i in range(0, num_files):
        for j in range(i+1, num_files):
            if min(nrc_mat[i][j], nrc_mat[j][i]) < ent_threshold:
                # num += 1
                simil_mat[i][j] = 1
                # simil_mat[j][i] = 1
    # print(num)
    return simil_mat


if find_simil_regions:
    nrc_mat = np.genfromtxt(ave_ent_file, dtype=float)
    simil_mat = build_simil_matrix(nrc_mat)

    # smashpp_bin = ''
    if os.name == 'posix':
        smashpp_bin = './smashpp'
    elif os.name == 'nt':
        smashpp_bin = '.\smashpp.exe'

    for i in range(0, len(simil_mat)):
        for j in range(i, len(simil_mat[0])):
            if i != j and simil_mat[i][j] == 1:
                cmd = smashpp_bin + ' -rm 11,0,1,0.95/8,0,1,0.9 -r ' + \
                    str(i + 1) + ' -t ' + str(j + 1) + ' -f 50 -th ' + \
                    str(ent_threshold) + ' -m 13 -rb 7 -re 3 -dp'
                os.popen(cmd).read()

if plot_simil:
    nrc_mat = np.genfromtxt(
        ave_ent_file + '_Chondrichthyes.tsv', skip_header=True)

    # for x in nrc_mat:
    #     print(*x, sep="\t")

    simil_mat = build_simil_matrix(nrc_mat)
    # # for x in simil_mat:
    # #     print(*x, sep=" ")

    # plt.matshow(nrc_mat)
    # # plt.colorbar()
    # # # plt.show()
    # # plt.savefig('nrc_mat_'+str(threshold)+'.pdf')

    plt.matshow(simil_mat)
    plt.colorbar()
    plt.show()
    # # plt.savefig('simil.'+str(threshold)+'.pdf')


#%%
