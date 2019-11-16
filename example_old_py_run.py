import os
import subprocess

# file_out = open("names.txt", "w+")
# counter = 0
# for file in os.listdir("."):
#     if file.endswith("seq"):
#         file_out.write(file+"\n")
#         counter = counter+1
#         os.rename(file, str(counter))

# num_files = 496
num_files = 2
num_first = 1
for idx_ref in range(num_first, num_files+1):
    for idx_tar in range(idx_ref+1, num_files+1):
        ref = str(idx_ref)
        tar = str(idx_tar)
        cmd = './smashpp -w 200 -rm 11,0,1,0.95/8,0,1,0.9 -r ' + ref + \
            ' -t ' + tar + ' -th 1.8 -m 60 -rb 15 -re 5'
        subprocess.call(cmd.split())


# for ref in os.listdir("."):
#     if ref.endswith(".seq"):
#         for tar in os.listdir("."):
#             if tar.endswith(".seq"):
#                 ref_name = os.path.join("", ref)
#                 tar_name = os.path.join("", tar)

#                 cmd_main = './smashpp -w 200 -rm 11,0,1,0.95/8,0,1,0.9 -r ' + \
#                     ref_name + ' -t ' + tar_name + ' -th 1.8;'

#                 ref_name_without_ext = ref_name[:9]
#                 tar_name_without_ext = tar_name[:9]
#                 cmd_visual = './smashpp -viz -vv -rt 1000 -tt 1000' + \
#                     ' -rn ' + ref_name_without_ext + \
#                     ' -tn ' + tar_name_without_ext + \
#                     ' -o ' + ref_name_without_ext + "-" + \
#                     tar_name_without_ext + '.svg ' + \
#                     ref_name + "-" + tar_name + ".pos"

#                 os.popen(cmd_main+cmd_visual).read()
