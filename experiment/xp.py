'''
Smash++
Morteza Hosseini, Diogo Pratas, Armando J. Pinho
{seyedmorteza,pratas,ap}@ua.pt
Copyright (C) 2018-2019, IEETA/DETI, University of Aveiro, Portugal
'''

'''
USER CAN CHANGE HERE BY SWITCHING False/True
'''
import shutil
import os

# Run (Benchmark)
RUN_SYNTH_SMALL = False
RUN_SYNTH_MEDIUM = False
RUN_SYNTH_LARGE = False
RUN_SYNTH_XLARGE = False
RUN_SYNTH_MUTATE = False
RUN_REAL_GGA18_MGA20 = False
RUN_REAL_GGA14_MGA16 = False
RUN_REAL_HS12_PT12 = False
RUN_REAL_PXO99A_MAFF311018 = False
RUN_SYNTH_COMPARE_SMASH = False  # Compare to Smash on synthetic data
RUN_REAL_COMPARE_SMASH = False  # Compare to Smash on real data
RUN_SYNTH_PERM_ORIGINAL = False
RUN_SYNTH_PERM_450000 = False
RUN_SYNTH_PERM_30000 = False
RUN_SYNTH_PERM_1000 = False
RUN_SYNTH_PERM_30 = False


###########################################################################
#                        D O   N O T   C H A N G E                        #
###########################################################################
'''
General
'''
sep = '/' if os.name == 'posix' else '\\'
path_bin = 'bin' + sep
path_data = 'dataset' + sep
current_dir = os.getcwd()
time_exe = '/usr/bin/time -v --output='
# execute('sudo chmod -R 777 bin/')


def execute(cmd):
    '''Execute in terminal'''
    os.popen(cmd).read()


def cat(file_in_names, file_out_name):
    '''Concatenate a number of files'''
    with open(file_out_name, 'w') as file_out:
        for file in file_in_names:
            with open(file) as file_in:
                for line in file_in:
                    file_out.write(line)


def append(file_in_name, file_out_name):
    '''Append a file to another file'''
    with open(file_in_name) as file_in, open(file_out_name, 'a') as file_out:
        for line in file_in:
            file_out.write(line)


def remove_all_ext(directory, extension):
    '''Remove all files in a directory with specific extension'''
    for file_name in os.listdir(directory):
        if file_name.endswith(extension):
            os.remove(os.path.join(directory, file_name))


def remove_start_with(directory, word):
    '''Remove all files in a directory of which their names start with word'''
    for file_name in os.listdir(directory):
        if file_name.startswith(word):
            os.remove(os.path.join(directory, file_name))


def remove_path(path):
    '''Remove a file or a directory, if it exists'''
    if os.path.exists(path):
        os.remove(path)


def make_path(path):
    '''Make a file or a directory, if it does not exist'''
    if not os.path.exists(path):
        os.makedirs(path)


def remove(directory, pattern):
    '''Remove files with specific pattern in their names from a directory'''
    from pathlib import Path
    for p in Path(directory).glob(pattern):
        p.unlink()


def bare_name(name):
    '''Extract file name from path'''
    return os.path.basename(name)


def file_size(file):
    '''Size of a file'''
    return os.path.getsize(file)


def extension_removed(file_with_extension):
    '''File name without extension'''
    return os.path.splitext(file_with_extension)[0]


def to_seconds(hms):
    '''Convert hour:minute:second to seconds'''
    vals = hms.split(':')
    if len(vals) == 2:
        h = 0
        m = int(vals[0])
        s = float(vals[1])
    elif len(vals) == 3:
        h = int(vals[0])
        m = int(vals[1])
        s = float(vals[2])
    return str(h * 3600 + m * 60 + s)


def tool_exists(name):
    '''Check if a tool is installed on this machine. Requires Python 3.3+'''
    from shutil import which
    return which(name) is not None


def extract(file, key):
    import re
    with open(file) as f:
        for line in f:
            if re.search(key, line):
                split = re.split(' ', line)
                return split[-1].strip()


def calc_mem(log_main, log_viz=''):
    key = 'Maximum resident'
    mem_main = extract(log_main, key)
    if not log_viz:
        return int(mem_main)
    else:
        mem_viz = extract(log_viz, key)
        return max(int(mem_main), int(mem_viz))


def calc_elapsed(log_main, log_viz=''):
    key = 'Elapsed'
    elapsed_main = to_seconds(extract(log_main, key))
    if not log_viz:
        return f"{float(elapsed_main):.2f}"
    else:
        elapsed_viz = to_seconds(extract(log_viz, key))
        return f"{float(elapsed_main) + float(elapsed_viz):.2f}"


def calc_user_time(log_main, log_viz=''):
    key = 'User'
    user_time_main = extract(log_main, key)
    if not log_viz:
        return f"{float(user_time_main):.2f}"
    else:
        user_time_viz = extract(log_viz, key)
        return f"{float(user_time_main) + float(user_time_viz):.2f}"


def calc_system_time(log_main, log_viz=''):
    key = 'System'
    system_time_main = extract(log_main, key)
    if not log_viz:
        return f"{float(system_time_main):.2f}"
    else:
        system_time_viz = extract(log_viz, key)
        return f"{float(system_time_main) + float(system_time_viz):.2f}"


class Dataset:
    synth_common_par = '-eh -eo -es -edb -rm 0 '
    smashpp_inv_rep = path_bin + 'smashpp-inv-rep '
    goose_mutatedna = path_bin + 'goose-mutatedna '
    goose_fastqsimulation = path_bin + 'goose-fastqsimulation '
    goose_permuteseqbyblocks = path_bin + 'goose-permuteseqbyblocks '
    path_data_synth = path_data + 'synth' + sep

    def __init__(self, key):
        self.key = key
        self._config()
        self.ref = self.ref_path + self.ref_name
        self.tar = self.tar_path + self.tar_name
        self.size = file_size(self.ref) + file_size(self.tar)

    def check_found(self, out):
        if out == 'not found':
            print(self.key + ' ' + out)
            exit()

    def _config(self):
        if self.key == 'synth_small':
            self.label = 'Small'
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefS'
            self.tar_name = 'TarS'
        elif self.key == 'synth_medium':
            self.label = 'Medium'
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefM'
            self.tar_name = 'TarM'
        elif self.key == 'synth_large':
            self.label = 'Large'
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefL'
            self.tar_name = 'TarL'
        elif self.key == 'synth_xlarge':
            self.label = 'XLarge'
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefXL'
            self.tar_name = 'TarXL'
        elif self.key == 'synth_mutate':
            self.label = 'Mutate'
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefMut'
            self.tar_name = 'TarMut'
        elif self.key == 'synth_comp_smash':
            self.label = 'CompSynth'
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefComp'
            self.tar_name = 'TarComp'
        elif self.key == 'synth_perm':
            self.label = 'PermOrig'
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefPerm'
            self.tar_name = 'TarPerm'
        elif self.key == 'synth_perm450000':
            block_size = '450000'
            self.label = 'Perm' + block_size
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefPerm' + block_size
            self.tar_name = 'TarPerm'
        elif self.key == 'synth_perm30000':
            block_size = '30000'
            self.label = 'Perm' + block_size
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefPerm' + block_size
            self.tar_name = 'TarPerm'
        elif self.key == 'synth_perm1000':
            block_size = '1000'
            self.label = 'Perm' + block_size
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefPerm' + block_size
            self.tar_name = 'TarPerm'
        elif self.key == 'synth_perm30':
            block_size = '30'
            self.label = 'Perm' + block_size
            self.category = 'Synthetic'
            self.ref_path = Dataset.path_data_synth
            self.tar_path = Dataset.path_data_synth
            self.ref_name = 'RefPerm' + block_size
            self.tar_name = 'TarPerm'
        elif self.key == 'real_gga18_mga20':
            self.label = 'GGA18_MGA20'
            self.category = 'Real'
            self.ref_path = path_data + 'bird' + sep + 'Gallus_gallus' + sep
            self.tar_path = \
                path_data + 'bird' + sep + 'Meleagris_gallopavo' + sep
            self.ref_name = '18.seq'
            self.tar_name = '20.seq'
        elif self.key == 'real_gga14_mga16':
            self.label = 'GGA14_MGA16'
            self.category = 'Real'
            self.ref_path = path_data + 'bird' + sep + 'Gallus_gallus' + sep
            self.tar_path = \
                path_data + 'bird' + sep + 'Meleagris_gallopavo' + sep
            self.ref_name = '14.seq'
            self.tar_name = '16.seq'
        elif self.key == 'real_hs12_pt12':
            self.label = 'HS12_PT12'
            self.category = 'Real'
            self.ref_path = path_data + 'mammalia' + sep + 'Homo_sapiens' + sep
            self.tar_path = \
                path_data + 'mammalia' + sep + 'Pan_troglodytes' + sep
            self.ref_name = '12.seq'
            self.tar_name = '12.seq'
        elif self.key == 'real_pxo99a_maff311018':
            self.label = 'PXO99A_MAFF311018'
            self.category = 'Real'
            self.ref_path = path_data + 'bacteria' + sep + \
                'Xanthomonas_oryzae_pv_oryzae' + sep
            self.tar_path = path_data + 'bacteria' + sep + \
                'Xanthomonas_oryzae_pv_oryzae' + sep
            self.ref_name = 'PXO99A.seq'
            self.tar_name = 'MAFF_311018.seq'
        elif self.key == 'real_comp_smash':
            self.label = 'CompReal'
            self.category = 'Real'
            self.ref_path = \
                path_data + 'fungi' + sep + 'Saccharomyces_cerevisiae' + sep
            self.tar_path = \
                path_data + 'fungi' + sep + 'Saccharomyces_paradoxus' + sep
            self.ref_name = 'VII.seq'
            self.tar_name = 'VII.seq'

    def _download_seq(id, output):
        '''Download a sequence using accession ID, with entrez direct'''
        seq = bare_name(output)
        message = 'Downloading ' + seq + ' with accession ' + str(id) + ' '
        print(message + '...', end='')
        execute('efetch -db nucleotide -format fasta ' +
                '-id "' + id + '" > ' + seq + '.fa')
        make_path(output[:len(output)-len(seq)])
        execute('bin/goose-fasta2seq < ' + seq + '.fa > ' + output + '.seq')
        os.remove(seq + '.fa')
        print('\r' + message + 'finished.')

    def _acquire_ref(self):
        if self.key == 'synth_small':
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5 -s 201  r_a')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.20,0.30,0.30,0.20,0.0 -ls 100 -n 5 -s 58  r_b')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 5 -s 15  r_c')
            cat(['r_a', 'r_b', 'r_c'], self.ref)
        elif self.key == 'synth_medium':
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 191  r_a')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250 -s 608  r_b')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.20,0.30,0.30,0.20,0.0 -ls 100 -n 250 -s 30  r_c')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250 -s 138  r_d')
            cat(['r_a', 'r_b', 'r_c', 'r_d'], self.ref)
        elif self.key == 'synth_large':
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 25000 -s 10101 r_a')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 25000 -s 10  r_b')
            cat(['r_a', 'r_b'], self.ref)
        elif self.key == 'synth_xlarge':
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.30,0.20,0.30,0.20,0.0 -ls 100 -n 250000 -s 1311 r_a')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.30,0.20,0.20,0.30,0.0 -ls 100 -n 250000 -s 7129 r_b')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 16  r_c')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 250000 -s 537  r_d')
            cat(['r_a', 'r_b', 'r_c', 'r_d'], self.ref)
        elif self.key == 'synth_mutate':
            for i in range(1, 60+1):
                execute(Dataset.goose_fastqsimulation +
                        Dataset.synth_common_par + ' -s ' + str(i) +
                        '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10 r_' + str(i))
                append('r_' + str(i), self.ref)
        elif self.key == 'synth_comp_smash':
            for i in range(0, 9 + 1):
                execute(Dataset.goose_fastqsimulation +
                        Dataset.synth_common_par + ' -s ' + str(i * 10 + 1) +
                        '-f 0.25,0.25,0.25,0.25,0.0 ' + '-ls 100 -n 1000 ' +
                        'r_' + str(i))
                append('r_' + str(i), self.ref)
        elif self.key == 'synth_perm':
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 198  r_a')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 110  r_b')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 30091 r_c')
            cat(['r_a', 'r_b', 'r_c'], self.ref)
        elif self.key == 'synth_perm450000':
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 198  r_a')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 110  r_b')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 30091 r_c')
            cat(['r_a', 'r_b', 'r_c'], self.ref_path + 'RefPerm')

            execute(Dataset.goose_permuteseqbyblocks + '-bs 450000 -s 6041 ' +
                    '< ' + self.ref_path + 'RefPerm > ' + self.ref)
        elif self.key == 'synth_perm30000':
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 198  r_a')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 110  r_b')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 30091 r_c')
            cat(['r_a', 'r_b', 'r_c'], self.ref_path + 'RefPerm')

            execute(Dataset.goose_permuteseqbyblocks + '-bs 30000 -s 328914 ' +
                    '< ' + self.ref_path + 'RefPerm > ' + self.ref)
        elif self.key == 'synth_perm1000':
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 198  r_a')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 110  r_b')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 30091 r_c')
            cat(['r_a', 'r_b', 'r_c'], self.ref_path + 'RefPerm')

            execute(Dataset.goose_permuteseqbyblocks + '-bs 1000 -s 564283 ' +
                    '< ' + self.ref_path + 'RefPerm > ' + self.ref)
        elif self.key == 'synth_perm30':
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 198  r_a')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 110  r_b')
            execute(Dataset.goose_fastqsimulation + Dataset.synth_common_par +
                    '-f 0.25,0.25,0.25,0.25,0.0 -ls 100 -n 10000 -s 30091 r_c')
            cat(['r_a', 'r_b', 'r_c'], self.ref_path + 'RefPerm')

            execute(Dataset.goose_permuteseqbyblocks + '-bs 1000 -s 900123 ' +
                    '< ' + self.ref_path + 'RefPerm > ' + self.ref)
        elif self.key == 'real_gga18_mga20':
            self._download_seq('CM000110', self.ref_path +
                               extension_removed(self.ref_name))
        elif self.key == 'real_gga14_mga16':
            self._download_seq('CM000106', self.ref_path +
                               extension_removed(self.ref_name))
        elif self.key == 'real_hs12_pt12':
            self._download_seq('NC_000012', self.ref_path +
                               extension_removed(self.ref_name))
        elif self.key == 'real_pxo99a_maff311018':
            self._download_seq('CP000967', self.ref_path +
                               extension_removed(self.ref_name))
        elif self.key == 'real_comp_smash':
            self._download_seq('NC_001139', self.ref_path +
                               extension_removed(self.ref_name))

    def _acquire_tar(self):
        if self.key == 'synth_small':
            execute(Dataset.smashpp_inv_rep + 'r_a t_c')
            execute(Dataset.goose_mutatedna + '-mr 0.02 < r_b > t_b')
            execute(Dataset.smashpp_inv_rep + 'r_c t_a')
            cat(['t_a', 't_b', 't_c'], self.tar)
        elif self.key == 'synth_medium':
            execute(Dataset.smashpp_inv_rep + 'r_a t_d')
            execute(Dataset.goose_mutatedna + '-mr 0.90 < r_b > t_c')
            shutil.copyfile('r_c', 't_a')
            execute(Dataset.goose_mutatedna + '-mr 0.03 < r_d > t_b')
            cat(['t_a', 't_b', 't_c', 't_d'], self.tar)
        elif self.key == 'synth_large':
            execute(Dataset.smashpp_inv_rep + 'r_a t_b')
            execute(Dataset.goose_mutatedna + '-mr 0.02 < r_b > t_a')
            cat(['t_a', 't_b'], self.tar)
        elif self.key == 'synth_xlarge':
            execute(Dataset.goose_mutatedna + '-mr 0.01 < r_a > t_a')
            execute(Dataset.smashpp_inv_rep + 'r_c t_b')
            shutil.copyfile('r_d', 't_c')
            execute(Dataset.smashpp_inv_rep + 'r_b t_d')
            cat(['t_a', 't_b', 't_c', 't_d'], self.tar)
        elif self.key == 'synth_mutate':
            for i in range(1, 60+1):
                execute(Dataset.goose_mutatedna + '-mr ' + str(i/100) +
                        ' < r_' + str(i) + ' > t_' + str(i))
                append('t_' + str(i), self.tar)
        elif self.key == 'synth_comp_smash':
            shutil.copyfile('r_0', 't_0')
            for i in range(1, 9 + 1):
                execute(Dataset.goose_mutatedna + '-mr ' + str(i/100) +
                        ' < r_' + str(i) + ' > t_' + str(i))
            for i in range(4, 6 + 1):
                execute(Dataset.smashpp_inv_rep + 't_' + str(i) +
                        ' t_' + str(i) + 'i')

            for i in range(0, 3 + 1):
                append('t_' + str(i), self.tar)
            for i in range(4, 6 + 1):
                append('t_' + str(i) + 'i', self.tar)
            for i in range(7, 9 + 1):
                append('t_' + str(i), self.tar)
        elif self.key == 'synth_perm':
            execute(Dataset.goose_mutatedna + '-mr 0.01 < r_a > t_c')
            execute(Dataset.smashpp_inv_rep + 'r_b t_b')
            execute(Dataset.goose_mutatedna + '-mr 0.02 < r_c > t_a')
            cat(['t_a', 't_b', 't_c'], self.tar)
        elif self.key == 'synth_perm450000':
            execute(Dataset.goose_mutatedna + '-mr 0.01 < r_a > t_c')
            execute(Dataset.smashpp_inv_rep + 'r_b t_b')
            execute(Dataset.goose_mutatedna + '-mr 0.02 < r_c > t_a')
            cat(['t_a', 't_b', 't_c'], self.tar)
        elif self.key == 'synth_perm30000':
            execute(Dataset.goose_mutatedna + '-mr 0.01 < r_a > t_c')
            execute(Dataset.smashpp_inv_rep + 'r_b t_b')
            execute(Dataset.goose_mutatedna + '-mr 0.02 < r_c > t_a')
            cat(['t_a', 't_b', 't_c'], self.tar)
        elif self.key == 'synth_perm1000':
            execute(Dataset.goose_mutatedna + '-mr 0.01 < r_a > t_c')
            execute(Dataset.smashpp_inv_rep + 'r_b t_b')
            execute(Dataset.goose_mutatedna + '-mr 0.02 < r_c > t_a')
            cat(['t_a', 't_b', 't_c'], self.tar)
        elif self.key == 'synth_perm30':
            execute(Dataset.goose_mutatedna + '-mr 0.01 < r_a > t_c')
            execute(Dataset.smashpp_inv_rep + 'r_b t_b')
            execute(Dataset.goose_mutatedna + '-mr 0.02 < r_c > t_a')
            cat(['t_a', 't_b', 't_c'], self.tar)
        elif self.key == 'real_gga18_mga20':
            self._download_seq('CM000981', self.tar_path +
                               extension_removed(self.tar_name))
        elif self.key == 'real_gga14_mga16':
            self._download_seq('CM000977', self.tar_path +
                               extension_removed(self.tar_name))
        elif self.key == 'real_hs12_pt12':
            self._download_seq('NC_036891', self.tar_path +
                               extension_removed(self.tar_name))
        elif self.key == 'real_pxo99a_maff311018':
            self._download_seq('AP008229', self.tar_path +
                               extension_removed(self.tar_name))
        elif self.key == 'real_comp_smash':
            self._download_seq('CP020299', self.tar_path +
                               extension_removed(self.tar_name))

    def acquire(self):
        if self.category == 'Real':
            if not os.path.exists(self.ref):
                self._acquire_ref()
            elif not os.path.exists(self.tar):
                self._acquire_tar()
        elif self.category == 'Synthetic':
            if not os.path.exists(self.ref) or not os.path.exists(self.tar):
                remove_path(self.ref)
                remove_path(self.tar)
                self._acquire_ref()
                self._acquire_tar()
                remove_start_with(current_dir, "r_")
                remove_start_with(current_dir, "t_")


class Benchmark:
    def __init__(self):
        self.result = []

    def write_result(self):
        from csv import writer
        with open('bench.csv', 'w') as bench_file:
            writer = writer(bench_file)
            writer.writerow(['Method', 'Dataset', 'Cat', 'Size.B',
                             'Memory.KB', 'Elapsed.s', 'User.s', 'System.s'])
            writer.writerows(self.result)

        # Move obtained results to the result/ directory
        make_path('result')
        for file in os.listdir(current_dir):
            if file.endswith('.csv') or file.endswith('.svg') or \
                    file.endswith('.pos') or file.endswith('.fil'):
                shutil.copy(file, 'result/')
                remove_path(file)


class Smashpp:
    log_main = 'log_main'
    log_viz = 'log_viz'
    smashpp_exe = '..' + sep + 'smashpp '

    def __init__(self, key):
        self.key = key
        self.dataset = Dataset(key)
        self._config()
        if not os.path.exists(Smashpp.smashpp_exe.strip()):
            Smashpp.smashpp_exe = 'bin' + sep + 'smashpp '

    def _config(self):
        if self.key == 'synth_small':
            self.par_main = '-l 3 -d 1 -f 100'
            self.par_viz = '-p 1 -rt 150 -tt 150 -l 1 -w 13 -vv -stat -o S.svg'
        elif self.key == 'synth_medium':
            self.par_main = '-l 3 -d 100 -f 50'
            self.par_viz = '-p 1 -l 1 -w 13 -vv -stat -o M.svg'
        elif self.key == 'synth_large':
            self.par_main = '-l 3 -d 100 -f 135'
            self.par_viz = '-p 1 -l 1 -w 13 -vv -stat -o L.svg'
        elif self.key == 'synth_xlarge':
            self.par_main = '-l 3 -d 100 -f 275'
            self.par_viz = '-p 1 -l 1 -w 13 -vv -stat -o XL.svg'
        elif self.key == 'synth_mutate':
            self.par_main = '-th 1.97 -l 3 -d 600 -f 100 -m 15000'
            self.par_viz = '-p 1 -l 1 -w 13 -rt 5000 -tt 5000 -vv -stat ' + \
                '-o Mut.svg'
        elif self.key == 'synth_comp_smash':
            self.par_main = '-th 1.7 -l 3 -f 1000 -d 10 -m 1 -sf'
            self.par_viz = '-p 1 -l 1 -w 13 -rn Ref -tn Tar -rt 100000 ' + \
                '-tt 100000 ' + '-stat -o CompSmash.svg'
        elif self.key == 'synth_perm':
            self.par_main = '-l 0 -f 10 -d 3000'
            self.par_viz = '-p 1 -l 6 -w 13 -s 35 -vv -rt 500000 ' + \
                '-tt 500000 -stat ' + '-o Perm.svg'
        elif self.key == 'synth_perm450000':
            self.par_main = '-l 0 -f 25 -d 3000 -ar'
            self.par_viz = '-p 1 -l 6 -w 13 -s 35 -vv -rt 500000 ' + \
                '-tt 500000 -stat ' + '-o Perm_450000.svg'
        elif self.key == 'synth_perm30000':
            self.par_main = '-l 0 -f 75 -d 1500 -ar'
            self.par_viz = '-p 1 -l 6 -w 13 -s 35 -vv -rt 500000 ' + \
                '-tt 500000 -stat ' + '-o Perm_30000.svg'
        elif self.key == 'synth_perm1000':
            self.par_main = '-l 0 -f 25 -d 300 -ar'
            self.par_viz = '-p 1 -l 6 -w 13  -rt 500000 -tt 500000 -stat ' + \
                '-o Perm_1000.svg'
        elif self.key == 'synth_perm30':
            self.par_main = '-l 0 -f 250 -d 1 -ar'
            self.par_viz = '-p 1 -l 6 -w 13 -s 35 -vv -rt 500000 ' + \
                '-tt 500000 -stat ' + '-o Perm_30.svg'
        elif self.key == 'real_gga18_mga20':
            self.par_main = '-rm 14,0,0.005,0.95/5,0,1,0.95 -f 130 ' + \
                '-m 500000 -d 2200 ' + '-th 1.9'
            self.par_viz = '-l 1 -p 1 -vv -tc 6 -rn "GGA 18" -tn "MGA 20" ' + \
                '-stat -o GGA18_MGA20.svg'
        elif self.key == 'real_gga14_mga16':
            self.par_main = '-rm 14,0,0.005,0.95/5,0,0.99,0.95 -f 200 ' + \
                '-d 1500 -th 1.95 ' + '-e 1.95 -m 400000'
            self.par_viz = '-l 1 -vv -p 1 -rn "GGA 14" -tn "MGA 16" ' + \
                '-rt 1500000 -tt 1500000 -stat -o GGA14_MGA16.svg'
        elif self.key == 'real_hs12_pt12':
            self.par_main = '-rm 14,0,0.001,0.95 -f 9000 -d 500 -th 1.9 ' + \
                '-m 100000'
            self.par_viz = '-l 1 -p 1 -vv -rn "HS 12" -tn "PT 12" ' + \
                '-rt 15000000 -tt 15000000 -stat -o HS12_PT12.svg'
        elif self.key == 'real_pxo99a_maff311018':
            self.par_main = '-rm 13,0,0.005,1 -f 150 -m 10000 -d 1000 ' + \
                '-th 1.55 -ar'
            self.par_viz = '-l 6 -vv -p 1 -rt 500000 -rn PXO99A ' + \
                '-tn "MAFF 311018" ' + '-stat -o PXO99A_MAFF_311018.svg'
        elif self.key == 'real_comp_smash':
            self.par_main = '-th 1.85 -l 3 -f 370 -d 100 -ar -sf'
            self.par_viz = '-p 1 -l 1 -w 13 -rn Sc.VII -tn Sp.VII -stat ' + \
                '-o Sc_Sp_smash.svg'

    def acquire_dataset(self):
        self.dataset.acquire()

    def run_main(self):
        execute(time_exe + Smashpp.log_main + ' ' + Smashpp.smashpp_exe +
                ' -r ' + self.dataset.ref + ' -t ' + self.dataset.tar + ' ' +
                self.par_main)

    def run_viz(self):
        execute(time_exe + Smashpp.log_viz + ' ' + Smashpp.smashpp_exe +
                ' -viz ' + self.par_viz + ' ' + bare_name(self.dataset.ref) +
                '.' + bare_name(self.dataset.tar) + '.pos')

    def run(self):
        self.run_main()
        self.run_viz()

    def bench(self, bench_result):
        method = 'Smash++'
        mem = calc_mem(Smashpp.log_main, Smashpp.log_viz)
        elapsed = calc_elapsed(Smashpp.log_main, Smashpp.log_viz)
        user_time = calc_user_time(Smashpp.log_main, Smashpp.log_viz)
        system_time = calc_system_time(Smashpp.log_main, Smashpp.log_viz)
        remove_path(Smashpp.log_main)
        remove_path(Smashpp.log_viz)
        bench_result.append([method, self.dataset.label, self.dataset.category,
                             self.dataset.size, mem,
                             elapsed, user_time, system_time])


class Smash:
    log_smash = 'log_smash'
    smash_exe = path_bin + 'smash '

    def __init__(self, key):
        self.key = key
        self.dataset = Dataset(key)
        self._config()

    def _config(self):
        if self.key == 'synth_comp_smash':
            self.ref = self.dataset.ref_name
            self.tar = self.dataset.tar_name
            self.par = '-t 1.7 -c 14 -d 9 -w 5000 -m 1 -nd'
        elif self.key == 'real_comp_smash':
            self.ref = 'Sc' + self.dataset.ref_name
            self.tar = 'Sp' + self.dataset.tar_name
            self.par = '-t 1.85 -c 14 -d 99 -w 15000 -m 1 -nd'

    def run(self):
        shutil.copyfile(self.dataset.ref, self.ref)
        shutil.copyfile(self.dataset.tar, self.tar)
        execute(time_exe + Smash.log_smash + ' ' + Smash.smash_exe + ' ' +
                self.par + ' ' + self.ref + ' ' + self.tar)
        os.remove(self.ref)
        os.remove(self.tar)
        remove_all_ext(current_dir, 'ext')
        remove_all_ext(current_dir, 'rev')
        remove_all_ext(current_dir, 'inf')
        remove(current_dir, '*.sys*x')

    def bench(self, bench_result):
        method = 'Smash'
        mem = calc_mem(Smash.log_smash)
        elapsed = calc_elapsed(Smash.log_smash)
        user_time = calc_user_time(Smash.log_smash)
        system_time = calc_system_time(Smash.log_smash)
        remove_path(Smash.log_smash)
        bench_result.append([method, self.dataset.label, self.dataset.category,
                             self.dataset.size, mem,
                             elapsed, user_time, system_time])


'''
Resolve dependencies
'''
if not tool_exists('efetch'):
    '''Entrez Direct'''
    print('Downloading and installing Entrez ...')
    execute('conda install -c bioconda entrez-direct --yes')
    print('Finished.')

if not os.path.exists('bin/goose-fasta2seq') or \
   not os.path.exists('bin/goose-fastqsimulation') or \
   not os.path.exists('bin/goose-mutatedna') or \
   not os.path.exists('bin/goose-permuteseqbyblocks'):
    # remove_start_with(current_dir, "goose-")
    # remove_path("goose")
    execute('git clone https://github.com/pratas/goose.git;' +
            'cd goose/src/;' +
            'make -j8;' +
            'cp goose-fasta2seq goose-fastqsimulation ' +
            'goose-mutatedna goose-permuteseqbyblocks ../../bin/')


'''
Run
'''
bench = Benchmark()


def run_smashpp(key):
    smashpp = Smashpp(key)
    smashpp.acquire_dataset()
    smashpp.run()
    smashpp.bench(bench.result)


def run_smash(key):
    smash = Smash(key)
    smash.run()
    smash.bench(bench.result)


if RUN_SYNTH_SMALL:
    run_smashpp('synth_small')

if RUN_SYNTH_MEDIUM:
    run_smashpp('synth_medium')

if RUN_SYNTH_LARGE:
    run_smashpp('synth_large')

if RUN_SYNTH_XLARGE:
    run_smashpp('synth_xlarge')

if RUN_SYNTH_MUTATE:
    run_smashpp('synth_mutate')

if RUN_REAL_GGA18_MGA20:
    run_smashpp('real_gga18_mga20')

if RUN_REAL_GGA14_MGA16:
    run_smashpp('real_gga14_mga16')

if RUN_REAL_HS12_PT12:
    run_smashpp('real_hs12_pt12')

if RUN_REAL_PXO99A_MAFF311018:
    run_smashpp('real_pxo99a_maff311018')

if RUN_SYNTH_COMPARE_SMASH:
    run_smashpp('synth_comp_smash')
    run_smash('synth_comp_smash')

if RUN_REAL_COMPARE_SMASH:
    run_smashpp('real_comp_smash')
    run_smash('real_comp_smash')

if RUN_SYNTH_PERM_ORIGINAL:
    run_smashpp('synth_perm')

if RUN_SYNTH_PERM_450000:
    run_smashpp('synth_perm450000')

if RUN_SYNTH_PERM_30000:
    run_smashpp('synth_perm30000')

if RUN_SYNTH_PERM_1000:
    run_smashpp('synth_perm1000')

if RUN_SYNTH_PERM_30:
    run_smashpp('synth_perm30')

bench.write_result()

# def run_bench(method, dataset, cat, size, func, arg=''):
#     import time
#     start_time = time.perf_counter()

#     def start(method, interval=0.001):
#         method_process = 'smashpp'
#         if method == 'Smash':
#             method_process = 'smash'
#         cmd_bash = \
#             'echo "0" > mem_ps; \
#             while true; do \
#                 ps -T aux | grep ' + method_process + ' ' + \
#                     '| grep -v "grep --color=auto" | awk \'{print $7}\' ' + \
#                     '| sort -V | tail -n 1 >> mem_ps; \
#                 sleep ' + str(interval) + '; \
#             done'
#         os.popen(cmd_bash)

#     def stop():
#         cmd_bash = \
#             'kill $! >/dev/null 2>&1; \
#             cat mem_ps | sort -V | tail -n 1 > peak_mem; \
#             rm -f mem_ps; \
#             kill `ps -T aux | grep dash | grep -v "grep --color=auto" ' + \
#                 '| awk \'{print $2}\'`;'
#         os.popen(cmd_bash)

#     # start(method=method)
#     memory = memory_profiler.memory_usage(func)
#     # stop()
#     end_time = time.perf_counter()
#     elapsed = f"{end_time - start_time:.2f}"
#     max_memory = f"{max(memory):.2f}"
#     bench_result.append([method, dataset, cat, size, elapsed, max_memory])
