import os

split_reads = False
fasta_to_seq = False
run_geco = False
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
    num_files = 3327
    first = 1
    os.popen("rm -f nrc").read()

    for i in range(first, num_files-first+1):
        cmd = 'printf \"' + str(i) + '\t\" >> nrc;' + \
            './geco -tm 6:1:0:0/0 -tm 10:10:1:0/0 -tm 14:50:1:3/10 -c 20' + \
            '-g 0.95 -v ' + str(i) + ' > log;' + \
            'grep bpb log | cut -d\' \' -f6 >> nrc'
        os.popen(cmd).read()

    os.popen("rm -f log *.co").read()
    # if os.path.exists("log"):
    #     os.remove("log")

if filter_nrc:
    threshold = 1.7
    cmd = 'rm -f nrc_filtered;' + \
        'awk \'{ if ($2 < ' + str(threshold) + ') { print } }\' nrc ' + \
        '>> nrc_filtered'
    os.popen(cmd).read()
