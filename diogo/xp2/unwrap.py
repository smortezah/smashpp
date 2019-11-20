import os
import subprocess

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
smashpp = '..' + sep + '..' + sep + 'smashpp '


def execute(cmd):
    os.popen(cmd).read()


ref_name = 'MT-CO1.fa'
tar_list = ['1']

for tar_name in tar_list:
    execute(smashpp + '-r ' + ref_name + ' -t ' +
            tar_name + ' -m 100')

    pos_file = open(ref_name + '.' + tar_name + '.pos')
    pos_file.readline()  # Skip header
    first_line = pos_file.readline().split('\t')
    beg_pos = int(first_line[4])
    end_pos = int(first_line[5])

    tar_bk_name = tar_name + '.bk'
    if os.path.exists(tar_bk_name):
        os.remove(tar_bk_name)
    os.rename(tar_name, tar_bk_name)
    tar_bk = open(tar_bk_name, 'r')
    tar_content = tar_bk.read()

    tar = open(tar_name, 'w')
    tar.write(str(tar_content[beg_pos:]))
    tar.write(str(tar_content[0:beg_pos]))
    # os.remove(tar_bk_name)
