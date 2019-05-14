import os

split_reads = False
fasta_to_seq = False
run_geco = True
build_simil_matrix = True
filter_nrc = False

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

if run_geco:
    num_files = 4  # 3327
    first = 1
    out_file = 'nrc'
    os.popen("rm -f " + out_file).read()

    for i in range(first, num_files - first + 1):
        for j in range(first, num_files - first + 1):
            cmd = './geco -rm 6:1:0:0/0 -rm 10:10:1:0/0 -rm 14:50:1:3/10 ' + \
                '-c 30 -g 0.95 -v -r ' + str(i) + ' ' + str(j) + ' > log;' + \
                'grep bpb log | cut -d\' \' -f6 >> ' + out_file
            os.popen(cmd).read()

    os.popen("rm -f log *.co").read()
    # if os.path.exists("log"):
    #     os.remove("log")

if build_simil_matrix:
    out_file = 'nrc'
    threshold = 2.0
    num_files_prev = 3  # 3327
    nrc_mat = [[0 for x in range(num_files_prev)]
               for y in range(num_files_prev)]
    simil_mat = [[0 for x in range(num_files_prev)]
                 for y in range(num_files_prev)]

    with open(out_file) as file:
        for i in range(0, num_files_prev):
            for j in range(0, num_files_prev):
                nrc_mat[i][j] = file.readline()[:-1]

    for i in range(0, num_files_prev):
        for j in range(i, num_files_prev):
            if min(float(nrc_mat[i][j]), float(nrc_mat[j][i])) < threshold:
                simil_mat[i][j] = 1

    print(nrc_mat)
    print(simil_mat)

if filter_nrc:
    threshold = 1.7
    cmd = 'rm -f nrc_filtered;' + \
        'awk \'{ if ($2 < ' + str(threshold) + ') { print } }\' nrc ' + \
        '>> nrc_filtered'
    os.popen(cmd).read()
