import os
import matplotlib.pyplot as plt

split_reads = False
fasta_to_seq = False
compress = True
plot_simil_matrix = False

num_files = 3  # 3327
nrc_file = 'nrc'
threshold = 1.9

if split_reads:
    main_file = 'mtDNA_Chordata_3327_22-03-2019.fasta'
    cmd = './goose-splitreads < ' + main_file
    os.popen(cmd).read()

if fasta_to_seq:
    for file in os.listdir("."):
        if file.endswith("fasta"):
            cmd = './goose-fasta2seq < ' + file + ' > ' + file[3:-6] + ';' \
                'rm -f ' + file
            os.popen(cmd).read()

if compress:
    first = 1
    nrc_mat = [[0 for x in range(num_files)]
               for y in range(num_files)]
    # os.popen("rm -f " + nrc_file).read()
    if os.path.exists(nrc_file):
        os.remove(nrc_file)

    for i in range(first, num_files - first + 2):
        for j in range(first, num_files - first + 2):
            cmd = './geco -rm 6:1:0:0/0 -rm 10:10:1:0/0 -rm 14:50:1:3/10 ' + \
                '-c 30 -g 0.95 -r ' + str(i) + ' ' + str(j) + ' > log'
            os.popen(cmd).read()

            with open('log', 'r') as log_file:
                for line in log_file:
                    line_list = line.split()
                    if len(line_list) > 5:
                        # print(line_list[5])
                        nrc_mat[i-1][j-1] = line_list[5]

    for x in nrc_mat:
        print(*x, sep="\t")

            # cmd = './geco -rm 6:1:0:0/0 -rm 10:10:1:0/0 -rm 14:50:1:3/10 ' + \
            #     '-c 30 -g 0.95 -v -r ' + str(i) + ' ' + str(j) + ' > log;' + \
            #     'grep bpb log | cut -d\' \' -f6 >> ' + nrc_file

    # # os.popen("rm -f log *.co").read()
    # for file in ["log", "*.co"]:
    #     if os.path.exists(file):
    #         os.remove(file)


def build_simil_matrix():
    nrc_mat = [[0 for x in range(num_files)]
               for y in range(num_files)]
    simil_mat = [[0 for x in range(num_files)]
                 for y in range(num_files)]

    with open(nrc_file) as file:
        for i in range(0, num_files):
            for j in range(0, num_files):
                nrc_mat[i][j] = file.readline()[:-1]

    for i in range(0, num_files):
        for j in range(i, num_files):
            if min(float(nrc_mat[i][j]), float(nrc_mat[j][i])) < threshold:
                simil_mat[i][j] = 1
                # simil_mat[j][i] = 1

    return simil_mat


if plot_simil_matrix:
    #     for x in nrc_mat:
    #         print(*x, sep="\t")
    #     for x in simil_mat:
    #         print(*x, sep=" ")
    simil_mat = build_simil_matrix()
    plt.matshow(simil_mat)
    plt.show()
