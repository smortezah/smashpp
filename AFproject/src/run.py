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
# compress_param = ' -rm 4:1000:1:0/0 -rm 8:1000:1:0/0 -rm 14:1000:1:3/10 ' + \
#     ' -rm 18:1000:1:5/10 -c 30 - g 0.95 '

sep = ''
if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
path_dataset = '..' + sep + 'dataset' + sep
path_result = '..' + sep + 'result' + sep
path_bin = '..' + sep + 'bin' + sep
prog = path_bin + 'GeCo2 '
nrc_file_name = 'nrc.csv'
nrc_ave_file_name = 'nrc_ave.csv'


def execute(cmd):
    os.popen(cmd).read()


def remove_file_if_exist(file):
    if os.path.exists(file):
        os.remove(file)


def remove_files_end_with(path, extension):
    if path[-1] != sep:
        path += sep
    for file in os.listdir(path):
        if file.endswith(extension):
            os.remove(path + file)


def create_dir_if_not_exit(path):
    if not os.path.exists(path):
        os.mkdir(path)


def grant_permission(path):
    execute('sudo chmod -R 777 ' + path)


def extension_removed(file_with_extension):
    return os.path.splitext(file_with_extension)[0]


def ave_matrix(mat):
    num_col = len(mat)
    ave_mat = [[0 for x in range(num_col)] for y in range(num_col)]
    for i in range(0, num_col):
        for j in range(i, num_col):
            if i == j:
                ave_mat[i][j] = mat[i][j]
            else:
                ave_mat[i][j] = ave_mat[j][i] = (mat[i][j] + mat[j][i]) / 2
    return ave_mat


def nrc_from_log_file(log_file_name):
    NRC = 0.0
    with open(log_file_name) as log_file:
        for line in log_file:
            line_list = line.split()
            if len(line_list) > 5:
                NRC = float(line_list[15])
                if NRC > 1.0:
                    NRC = 1.0
    return NRC


if find_simil_seqs:
    remove_file_if_exist('log')
    remove_files_end_with(path_dataset, '.co')
    create_dir_if_not_exit(path_result)
    grant_permission(path_bin)

    print('Finding similar sequences ...')
    in_file_path = path_dataset
    in_file_list = os.listdir(in_file_path)
    out_file_name = nrc_file_name
    remove_file_if_exist(path_result + out_file_name)

    with open(path_result + out_file_name, "w") as out_file:
        for file in in_file_list:
            file_name = extension_removed(file)
            out_file.write(',' + file_name)
        out_file.write('\n')

        for i in range(len(in_file_list)):
            file_name = extension_removed(in_file_list[i])
            out_file.write(file_name)
            for j in range(len(in_file_list)):
                ref = in_file_path + in_file_list[i]
                tar = in_file_path + in_file_list[j]
                nrc = 0.0
                if i != j:
                    execute(prog + compress_param + ' -r ' + ref + ' ' +
                            tar + ' > log')
                    nrc = nrc_from_log_file('log')
                out_file.write(',' + str(nrc))
                remove_file_if_exist(tar + '.co')
            out_file.write('\n')
    remove_file_if_exist('log')
    print('Finished.')

if make_nrc_ave:
    print('Making NRC average ...')
    with open(path_result + nrc_file_name) as nrc_file:
        header = nrc_file.readline()[:-1].split(",")[1:]
        nrc_file.seek(0)
        nrc_mat = np.genfromtxt(nrc_file, skip_header=True, delimiter=",",
                                usecols=range(1, len(header) + 1))
    remove_file_if_exist(path_result + nrc_ave_file_name)
    nrc_ave = ave_matrix(nrc_mat)
    with open(path_result + nrc_ave_file_name, "w") as nrc_ave_file:
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
    with open(path_result + 'nrc_ave.csv') as nrc_ave, \
            open(path_result + 'dist.phy', 'w') as dist:
        reader = csv.reader(nrc_ave)
        num_seqs = len(reader.__next__()) - 1
        data = []
        for _ in reader:
            data.append(_)
        writer = csv.writer(dist, delimiter='\t')
        writer.writerow(["", num_seqs])
        writer.writerows(data)
    print('Finished.')
