import os
import matplotlib.pyplot as plt
import numpy as np

prepare_data = True
find_simil_seqs = False
find_simil_regions = False
plot_simil = False

main_file = 'mtDNA_Chordata_3327_22-03-2019.fasta'
data_in_path = 'Taxonomy_Chordata_NCBI_Diogo_06-05-2019'
num_files = 3327
ave_ent_file = 'ent'
threshold = 0.2  # in (0, 1]
ent_threshold = 8 * threshold

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'


def execute(cmd):
    os.popen(cmd).read()


if prepare_data:
    data_out_path = 'dataset'
    if not os.path.exists(data_out_path):
        os.mkdir(data_out_path)

    print('Linearizing DNA reads ...', end="\r")
    cox1_pos_file = open('COX1_pos', 'w+')
    cox1_pos_file.write('Name' + '\t' + 'Organism' +
                        '\t' + 'COX1_beg' + '\t' + 'COX1_end' + '\n')
    for input_file_name in os.listdir(data_in_path):
        in_file = open(data_in_path + sep + input_file_name)
        for line in in_file:
            if line[2:10].lower() == 'sequence':
                sequence = line[15:-3]
            if line[2:10].lower() == 'organism':
                organism = line[15:-3]
            if line[2:6].lower() == 'cox1':
                line_separated = line.split(', ')
                cox1_beg = line_separated[1][1:-1]
                cox1_end = line_separated[2][1:-3]
                break

        seq_file = open(data_out_path + sep + input_file_name + '.seq', 'w')
        seq_file.write(sequence[int(cox1_beg)-1:].upper())
        seq_file.write(sequence[0:int(cox1_beg)-1].upper() + '\n')
        seq_file.close()

        cox1_pos_file.write(input_file_name + '\t' + organism +
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
    first = 1
    if os.path.exists(ave_ent_file):
        os.remove(ave_ent_file)

    out_file = open(ave_ent_file, "w")
    for i in range(first, num_files - first + 2):
        for j in range(first, num_files - first + 2):
            cmd = './geco -rm 6:1:0:0/0 -rm 10:10:1:0/0 -rm 14:50:1:3/10 ' + \
                '-c 30 -g 0.95 -r ' + str(i) + ' ' + str(j) + ' > log'
            os.popen(cmd).read()

            with open('log', 'r') as log_file:
                for line in log_file:
                    line_list = line.split()
                    if len(line_list) > 5:
                        out_file.write(str(line_list[5]) + "\t")
        out_file.write("\n")
    out_file.close()

    for file in ["log", "*.co"]:
        if os.path.exists(file):
            os.remove(file)


def build_simil_matrix(nrc_mat):
    # num = 0
    simil_mat = [[0 for x in range(num_files)] for y in range(num_files)]
    for i in range(0, num_files):
        for j in range(i, num_files):
            if i != j:
                if min(nrc_mat[i][j], nrc_mat[j][i]) < ent_threshold:
                    # num += 1
                    simil_mat[i][j] = 1
                    # simil_mat[j][i] = 1
    # print(num)
    return simil_mat


if find_simil_regions:
    nrc_mat = np.genfromtxt(ave_ent_file, dtype=float)
    simil_mat = build_simil_matrix(nrc_mat)

    smashpp_bin = ''
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
    nrc_mat = np.genfromtxt(ave_ent_file, dtype=float)
    # for x in nrc_mat:
    #     print(*x, sep="\t")

    simil_mat = build_simil_matrix(nrc_mat)
    # for x in simil_mat:
    #     print(*x, sep=" ")

    # plt.matshow(nrc_mat)
    # plt.colorbar()
    # # plt.show()
    # plt.savefig('nrc_mat_'+str(threshold)+'.pdf')

    plt.matshow(simil_mat)
    plt.colorbar()
    plt.show()
    # plt.savefig('simil.'+str(threshold)+'.pdf')
