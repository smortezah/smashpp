import os

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
smashpp = '..' + sep + 'smashpp '


def execute(cmd):
    os.popen(cmd).read()


ref = 'MT-CO1.fa'
tar_list = ['1']

for tar in tar_list:
    # execute(smashpp + '-r ' + ref + ' -t ' + tar)

    with open(ref + '.' + tar + '.pos') as pos_file:
        first_line = pos_file.readline()
        print(first_line[5])
