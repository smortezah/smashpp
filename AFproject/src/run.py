import os
import shutil
import numpy as np
import csv

find_simil_seqs = True
make_nrc_ave = True
make_Phylip_distance_matrix = True

# MUST NOT use '-v' option
compress_param = ' -rm 4:1000:1:30:0.85/0:1000:0.8 ' + \
    ' -rm 8:1000:0:30:0.85/1:1000:0.8 -rm 12:1000:0:30:0.9/2:1000:0.8 ' + \
    ' -rm 14:1000:0:30:0.9/2:1000:0.8 -rm 16:1000:0:30:0.95/3:1000:0.8 ' + \
    ' -rm 18:1000:0:30:0.95/4:1000:0.8 '
# compress_param = ' -rm 4:1000:1:0/0 -rm 8:1000:1:0/0 -rm 14:1000:1:3/10 -rm 18:1000:1:5/10 -c 30 - g 0.95 '

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
dataset_path = '..' + sep + 'dataset' + sep
result_path = '..' + sep + 'result' + sep
bin_path = '..' + sep + 'bin' + sep
geco2 = bin_path + 'GeCo2 '


def execute(cmd):
    os.popen(cmd).read()


def remove_file_if_exist(file):
    if os.path.exists(file):
        os.remove(file)


def create_dir_if_not_exit(path):
    if not os.path.exists(path):
        os.mkdir(path)


if find_simil_seqs:
    remove_file_if_exist('log')
    for file in os.listdir(dataset_path):
        if file.endswith('.co'):
            os.remove(dataset_path + file)
    create_dir_if_not_exit(result_path)
    # execute('sudo chmod -R 777 ' + bin_path)

    in_file_path = dataset_path
    in_file_list = os.listdir(in_file_path)
    out_file_name = 'nrc.csv'

    print('Finding similar sequences ...')
    remove_file_if_exist(result_path + out_file_name)
    with open(result_path + out_file_name, "w") as out_file:
        for file in in_file_list:
            file_name = os.path.splitext(file)[0]
            out_file.write(',' + file_name)
        out_file.write('\n')

        for i in range(len(in_file_list)):
            file_name = os.path.splitext(in_file_list[i])[0]
            out_file.write(file_name)
            for j in range(len(in_file_list)):
                ref = in_file_path + in_file_list[i]
                tar = in_file_path + in_file_list[j]
                if i != j:
                    execute(geco2 + compress_param + ' -r ' + ref + ' ' +
                            tar + ' > log')
                    with open('log', 'r') as log_file:
                        for line in log_file:
                            line_list = line.split()
                            if len(line_list) > 5:
                                NRC = float(line_list[15])
                                if NRC > 1:
                                    NRC = 1
                                out_file.write(',' + str(NRC))
                else:
                    out_file.write(',0')
                remove_file_if_exist(tar + '.co')
            out_file.write('\n')
    remove_file_if_exist('log')
    print('Finished.')

if make_nrc_ave:
    print('Making NRC average ...')
    with open(result_path + 'nrc.csv') as nrc_file:
        header = nrc_file.readline()[:-1].split(",")[1:]
        nrc_file.seek(0)
        nrc_mat = np.genfromtxt(nrc_file, skip_header=True, delimiter=",",
                                usecols=range(1, len(header) + 1))
    nrc_ave_name = 'nrc_ave.csv'
    remove_file_if_exist(result_path + nrc_ave_name)

    num_files = len(nrc_mat)
    nrc_ave = [[0 for x in range(num_files)] for y in range(num_files)]
    for i in range(0, num_files):
        for j in range(i, num_files):
            if i == j:
                nrc_ave[i][j] = nrc_mat[i][j]
            else:
                nrc_ave[i][j] = nrc_ave[j][i] = (
                    nrc_mat[i][j] + nrc_mat[j][i]) / 2

    with open(result_path + nrc_ave_name, "w") as nrc_ave_file:
        for i in range(len(header)):
            nrc_ave_file.write(',' + header[i])
        nrc_ave_file.write('\n')
        for i in range(len(header)):
            nrc_ave_file.write(header[i])
            for j in range(len(header)):
                nrc_ave_file.write(',' + str(nrc_ave[i][j]))
            nrc_ave_file.write('\n')
    print('Finished.')

if make_Phylip_distance_matrix:
    print('Making Phylip distance matrix ...')
    with open(result_path + 'nrc_ave.csv') as nrc_ave, \
            open(result_path + 'dist.phy', 'w') as dist:
        reader = csv.reader(nrc_ave)
        num_seqs = len(reader.__next__()) - 1
        data = []
        for _ in reader:
            data.append(_)
        writer = csv.writer(dist, delimiter='\t')
        writer.writerow(["", num_seqs])
        writer.writerows(data)
    print('Finished.')
