import matplotlib.pyplot as plt
import numpy as np
import subprocess as sp

# f = open("result", "w")
# args = "./smashpp -v -m 17,0,1,0 -r ref -t 5".split()
# popen = sp.Popen(args, stderr=f)
# popen.wait()


# import os
# import psutil
# process = psutil.Process(os.getpid())
# print(process.memory_info().rss)


# x, y = np.loadtxt('smashpp-3', delimiter='\t', unpack=True)
# plt.plot(x, y, '-o', label='MM')
# x, y = np.loadtxt('smashpp-mut', delimiter='\t', unpack=True)
# plt.plot(x, y, '-o', label='MM & STMM - Smash++')
# x, y = np.loadtxt('geco-mut', delimiter='\t', unpack=True)
# plt.plot(x, y, '-o', label='MM & STMM - GeCo')
# plt.xlabel('Mutation rate (%)')
# plt.ylabel('Average entropy (bps)')
# plt.legend()
# plt.grid(linewidth=.3)
# plt.show()


x=range(5000)
y = np.loadtxt('REF_TAR.prf')
plt.plot(x, y, label='Smash++')
plt.xlabel('')
plt.ylabel('Average entropy (bps)')
plt.legend()
plt.grid(linewidth=.3)
plt.show()

x=range(5000)
y = np.loadtxt('TAR.iae')
plt.plot(x, y, label='Geco')
plt.xlabel('')
plt.ylabel('Average entropy (bps)')
plt.legend()
plt.grid(linewidth=.3)
plt.show()

