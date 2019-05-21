import os
# import matplotlib.pyplot as plt
# import numpy as np

simulation = True

if os.name == 'posix':
    sep = '/'
elif os.name == 'nt':
    sep = '\\'
smashpp_bin = '.' + sep + 'smashpp'

if simulation:
    path_data = 'dataset' + sep + 'sim'
    path_bin = '..' + sep

    cmd = path_bin + smashpp_bin
    os.popen(cmd).read()
