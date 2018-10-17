import matplotlib.pyplot as plt
from matplotlib.pyplot import figure
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


# figure(num=None, figsize=(8, 8), dpi=80, facecolor='w', edgecolor='k')
# plt.subplot(211)
# r, g, s = np.loadtxt('syn_51', delimiter='\t', unpack=True)
# plt.plot(r, g, '-o', label='GeCo')
# plt.plot(r, s, '-o', label='Smash++')
# plt.title('Synthetic data (51 MB), without "N"')
# plt.xlabel('Mutation rate (%)')
# plt.ylabel('Average entropy (bps)')
# plt.legend()
# plt.grid(linewidth=.3)
#
# plt.subplot(212)
# r, g, s = np.loadtxt('syn_21', delimiter='\t', unpack=True)
# plt.plot(r, g, '-o', label='GeCo')
# plt.plot(r, s, '-o', label='Smash++')
# plt.title('Synthetic data (21 MB), without "N"')
# plt.xlabel('Mutation rate (%)')
# plt.ylabel('Average entropy (bps)')
# plt.legend()
# plt.grid(linewidth=.3)
# plt.show()


# figure(num=None, figsize=(7, 7), dpi=80, facecolor='w', edgecolor='k')
# plt.subplot(221)
# g, s = np.loadtxt('hs18-pt18_STMM', delimiter='\t', unpack=True)
# plt.bar('GeCo', height=g, width=0.4)
# plt.bar('Smash++', height=s, width=0.4)
# plt.title('With STMM - ref:HS18, tar:PT18')
# plt.ylabel('Average entropy (bps)')
#
# plt.subplot(222)
# g, s = np.loadtxt('pt18-hs18_STMM', delimiter='\t', unpack=True)
# plt.bar('GeCo', height=g, width=0.4)
# plt.bar('Smash++', height=s, width=0.4)
# plt.title('With STMM - ref:PT18, tar:HS18')
# # plt.ylabel('Average entropy (bps)')
#
# plt.subplot(223)
# g, s = np.loadtxt('pt18-hs18', delimiter='\t', unpack=True)
# plt.bar('GeCo', height=g, width=0.4)
# plt.bar('Smash++', height=s, width=0.4)
# plt.title('Without STMM - ref:PT18, tar:HS18')
# plt.ylabel('Average entropy (bps)')
#
# plt.subplot(224)
# g, s = np.loadtxt('hs18-pt18', delimiter='\t', unpack=True)
# plt.bar('GeCo', height=g, width=0.4)
# plt.bar('Smash++', height=s, width=0.4)
# plt.title('Without STMM - ref:HS18, tar:PT18')
# # plt.ylabel('Average entropy (bps)')
# plt.show()


ref='REF'#'ref11'#'hs18'#
tar='TAR'#'45'#'pt18'#
figure(num=None, figsize=(12, 8))

plt.subplot(211)
x = np.loadtxt(ref+'_'+tar+'.prf')
plt.plot(x)
plt.title('Smash++')
plt.grid(linewidth=.3)

plt.subplot(212)
y = np.loadtxt(tar+'.iae')
plt.plot(y)
plt.title('GeCo')
plt.grid(linewidth=.3)
plt.show()

# plt.subplot(211)
# x, y = np.loadtxt(ref+'_'+tar+'.fil', delimiter='\t', unpack=True)
# plt.plot(x, y, '-o')
# plt.title('Smash++')
# plt.grid(linewidth=.3)
#
# plt.subplot(212)
# x, y = np.loadtxt(tar+'.fil', delimiter='\t', unpack=True)
# plt.plot(x, y, '-o')
# plt.title('GeCo')
# plt.grid(linewidth=.3)
# plt.show()


# x=range(5000)
# y = np.loadtxt('REF_TAR.prf')
# plt.plot(x, y, label='Smash++')
# plt.xlabel('')
# plt.ylabel('Average entropy (bps)')
# plt.legend()
# plt.grid(linewidth=.3)
# plt.show()
#
# x=range(5000)
# y = np.loadtxt('TAR.iae')
# plt.plot(x, y, label='Geco')
# plt.xlabel('')
# plt.ylabel('Average entropy (bps)')
# plt.legend()
# plt.grid(linewidth=.3)
# plt.show()