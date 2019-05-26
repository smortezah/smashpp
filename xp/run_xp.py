import os
# import matplotlib.pyplot as plt
# import numpy as np

make_dataset_synth = True
simulation = False

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
smashpp_bin = '.' + sep + 'smashpp'


def execute(cmd):
    os.popen(cmd).read()


if make_dataset_synth:

if simulation:
    path_data = 'dataset' + sep + 'sim' + sep
    path_bin = '..' + sep
    gen_par = ' -w 15 -s 60 -vv '
    small = False
    medium = False
    large = False
    extra_large = False
    mutation = True

    if small:
        ref = 'RefS'
        tar = 'TarS'
        out = 'S.svg'
        execute(path_bin + smashpp_bin + ' -r ' + path_data +
                ref + ' -t ' + path_data + tar + ' -l 3 -f 45')
        execute(path_bin + smashpp_bin + ' -viz ' +
                '-p 1 -b 200 -f 70 -o ' + out + gen_par + ref + '.' + tar + '.pos')

    if medium:
        ref = 'RefM'
        tar = 'TarM'
        out = 'M.svg'
        execute(path_bin + smashpp_bin + ' -r ' + path_data +
                ref + ' -t ' + path_data + tar + ' -l 3 -f 100')
        execute(path_bin + smashpp_bin +
                ' -viz -p 1 -b 50 -f 60 -o ' + out + gen_par + ref + '.' + tar + '.pos')

    if large:
        ref = 'RefL'
        tar = 'TarL'
        out = 'L.svg'
        execute(path_bin + smashpp_bin + ' -r ' + path_data +
                ref + ' -t ' + path_data + tar + ' -l 3 -f 135')
        execute(path_bin + smashpp_bin + ' -viz -p 1 -b 50 -f 60 -o ' +
                out + gen_par + ref + '.' + tar + '.pos')

    if extra_large:
        ref = 'RefXL'
        tar = 'TarXL'
        out = 'XL.svg'
        execute(path_bin + smashpp_bin + ' -r ' + path_data +
                ref + ' -t ' + path_data + tar + ' -l 3 -f 275')
        execute(path_bin + smashpp_bin + ' -viz -p 1 -m 20000000 -o ' +
                out + gen_par + ref + '.' + tar + '.pos')

    if mutation:
        ref = 'RefMut'
        tar = 'TarMut'
        out = 'Mut.svg'
        execute(path_bin + smashpp_bin + ' -r ' + path_data +
                ref + ' -t ' + path_data + tar + ' -th 2 -l 5 -f 25000 -m 15000')
        execute(path_bin + smashpp_bin + ' -viz -p 1 -b 2 -f 50 -o ' +
                out + gen_par + ref + '.' + tar + '.pos')
