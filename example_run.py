##########   D O   N O T   C H A N G E   ##########
import os

# if os.name == 'posix':
#     sep = '/'
# elif os.name == 'nt':
#     sep = '\\'
# current_dir = os.getcwd()
# path_data = 'dataset' + sep + 'sim' + sep
# path_bin = 'bin' + sep
# goose_fastqsimulation = path_bin + 'goose-fastqsimulation '
# goose_mutatedna = path_bin + 'goose-mutatedna '
smashpp = '.' + sep + 'smashpp '
# smashpp_inv_rep = path_bin + 'smashpp-inv-rep '
# sim_common_par = ' -w 15 -s 60 -vv '


def execute(cmd):
    os.popen(cmd).read()


def install_samshpp():
    cmd = 'cd ..;'
    if os.name == 'posix':
        cmd += 'sh install.sh;' + \
            'cp smashpp example/;'
    elif os.name == 'nt':
        cmd += '.\\install.bat;' + \
            'cp .\\smashpp.exe .\\example\\ -Force;'
    cmd += 'cd example'
    execute(cmd)


# Input number of the example
num_example_str = input("Enter the number of example: ")
if num_example_str:
    num_example = int(num_example_str)
else:
    num_example = 1

viz_out = 'x' + num_example

Print('Installing Smash++')
install_samshpp()
###################################################


# User settings (can be changed)
ref = 'ref'
tar = 'tar'


if num_example == 1:
    print("===[ Example 1 ]====================================")
    execute(smashpp + '-r ' + ref + ' -t ' + tar + ' -l 3 -f 45')
    out = 'x1'
    execute(smashpp + '-viz -p 1 -b 200 -f 70 -w 15 -s 60 -vv -o ' +
            viz_out + ref + '.' + tar + '.pos')

elif num_example == 2:
    print("bye")

# # file_out = open("names.txt", "w+")
# # counter = 0
# # for file in os.listdir("."):
# #     if file.endswith("seq"):
# #         file_out.write(file+"\n")
# #         counter = counter+1
# #         os.rename(file, str(counter))

# # num_files = 496
# num_files = 2
# num_first = 1
# for idx_ref in range(num_first, num_files+1):
#     for idx_tar in range(idx_ref+1, num_files+1):
#         ref = str(idx_ref)
#         tar = str(idx_tar)
#         cmd = './smashpp -w 200 -rm 11,0,1,0.95/8,0,1,0.9 -r ' + ref + \
#             ' -t ' + tar + ' -th 1.8 -m 60 -rb 15 -re 5'
##             ' -t ' + tar + ' -th 1.8 -m 60 -rb 15 -re 5 -dp'
#         os.popen(cmd).read()


# # for ref in os.listdir("."):
# #     if ref.endswith(".seq"):
# #         for tar in os.listdir("."):
# #             if tar.endswith(".seq"):
# #                 ref_name = os.path.join("", ref)
# #                 tar_name = os.path.join("", tar)

# #                 cmd_main = './smashpp -w 200 -rm 11,0,1,0.95/8,0,1,0.9 -r ' + \
# #                     ref_name + ' -t ' + tar_name + ' -th 1.8;'

# #                 ref_name_without_ext = ref_name[:9]
# #                 tar_name_without_ext = tar_name[:9]
# #                 cmd_visual = './smashpp -viz -vv -rt 1000 -tt 1000' + \
# #                     ' -rn ' + ref_name_without_ext + \
# #                     ' -tn ' + tar_name_without_ext + \
# #                     ' -o ' + ref_name_without_ext + "-" + \
# #                     tar_name_without_ext + '.svg ' + \
# #                     ref_name + "-" + tar_name + ".pos"

# #                 os.popen(cmd_main+cmd_visual).read()
