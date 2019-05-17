import os
import matplotlib.pyplot as plt
import numpy as np

prepare_data = False
compress = False
plot_simil = True

main_file = 'mtDNA_Chordata_3327_22-03-2019.fasta'
num_files = 3327
nrc_file = 'nrc'
threshold = 1.95

if prepare_data:
    # Split reads
    cmd = './goose-splitreads < ' + main_file
    os.popen(cmd).read()

    # Convert fasta to seq
    for file in os.listdir("."):
        if file.endswith("fasta"):
            cmd = './goose-fasta2seq < ' + file + ' > ' + file[3:-6]
            os.popen(cmd).read()
            if os.path.exists(file):
                os.remove(file)

if compress:
    first = 1
    if os.path.exists(nrc_file):
        os.remove(nrc_file)

    out_file = open(nrc_file, "w")
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

if plot_simil:
    def build_simil_matrix(nrc_mat):
        simil_mat = [[0 for x in range(num_files)] for y in range(num_files)]
        for i in range(0, num_files):
            for j in range(i, num_files):
                if i != j:
                    if min(nrc_mat[i][j], nrc_mat[j][i]) < threshold:
                        simil_mat[i][j] = 1
                        # simil_mat[j][i] = 1
        return simil_mat

    nrc_mat = np.genfromtxt(nrc_file, dtype=float)
    # for x in nrc_mat:
    #     print(*x, sep="\t")

    simil_mat = build_simil_matrix(nrc_mat)
    # for x in simil_mat:
    #     print(*x, sep=" ")

    # plt.matshow(nrc_mat)
    # plt.colorbar()
    # # plt.show()
    # plt.savefig('nrc_mat_'+threshold+'.pdf')

    plt.matshow(simil_mat)
    plt.colorbar()
    # plt.show()
    plt.savefig('simil.'+str(threshold)+'.pdf')