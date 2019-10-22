import os
import shutil
import numpy as np

find_simil_seqs = True
make_nrc_ave = False


if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
dataset_path = '..' + sep + 'dataset' + sep
result_path = '..' + sep + 'result' + sep
bin_path = '..' + sep + 'bin' + sep
geco = bin_path + 'GeCo '


def execute(cmd):
    os.popen(cmd).read()


if find_simil_seqs:
    if os.path.exists('log'):
        os.remove('log')
    for file in os.listdir(dataset_path):
        if file.endswith('.co'):
            os.remove(dataset_path + file)
    if not os.path.exists(result_path):
        os.mkdir(result_path)
    execute('sudo chmod -R 777 ' + bin_path)

    in_file_path = dataset_path
    in_file_list = os.listdir(in_file_path)
    out_file_name = 'nrc.tsv'

    print('Finding similar sequences ...')
    if os.path.exists(result_path + out_file_name):
        os.remove(result_path + out_file_name)
    out_file = open(result_path + out_file_name, "w")
    for file in in_file_list:
        out_file.write('\t' + file[:-4])
    out_file.write('\n')

    first = 0
    for i in range(first, len(in_file_list)):
        out_file.write(in_file_list[i][:-4])
        for j in range(first, len(in_file_list)):
            ref = in_file_path + in_file_list[i]
            tar = in_file_path + in_file_list[j]
            # MUST NOT use '-v' option with GeCo
            execute(geco + '-rm 6:1:0:0/0 -rm 10:10:1:0/0 -rm 14:50:1:3/10 ' +
                    '-c 30 -g 0.95 -r ' + ref + ' ' + tar + ' > log')

            log_file = open('log', 'r')
            for line in log_file:
                line_list = line.split()
                if len(line_list) > 5:
                    NRC = float(line_list[15])
                    if NRC > 1:
                        NRC = 1
                    out_file.write('\t' + str(NRC))
            
            if os.path.exists(tar + '.co'):
                os.remove(tar + '.co')
        out_file.write('\n')
        
    if os.path.exists('log'):
        os.remove('log')
    print('Finished.')


def build_nrc_ave(nrc_mat):
    num_files = len(nrc_mat)
    ave_mat = [[0 for x in range(num_files)] for y in range(num_files)]
    for i in range(0, num_files):
        for j in range(i, num_files):
            if i == j:
                ave_mat[i][j] = nrc_mat[i][j]
            else:
                ave_mat[i][j] = ave_mat[j][i] = (nrc_mat[i][j] + nrc_mat[j][i]) / 2
    return ave_mat


if make_nrc_ave:
    print('Making NRC average ...')
    nrc_file = open(result_path + 'nrc.tsv')
    header = nrc_file.readline().split()
    nrc_file.seek(0)
    nrc_mat = np.genfromtxt(nrc_file, skip_header=True,
                            usecols=range(1, len(header) + 1))
    nrc_ave_name = 'nrc_ave.tsv'
    if os.path.exists(result_path + nrc_ave_name):
        os.remove(result_path + nrc_ave_name)

    nrc_ave = build_nrc_ave(nrc_mat)
    nrc_ave_file = open(result_path + nrc_ave_name, "w")
    for i in range(len(header)):
        nrc_ave_file.write('\t' + header[i])
    nrc_ave_file.write('\n')
    for i in range(len(header)):
        nrc_ave_file.write(header[i])
        for j in range(len(header)):
            nrc_ave_file.write('\t' + str(nrc_ave[i][j]))
        nrc_ave_file.write('\n')
    print('Finished.')
