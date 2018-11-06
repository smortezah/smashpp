import matplotlib.pyplot as plt
import scipy as scipy
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


# ref='ref'#'REF'#'ref11'#'hs18'#
# tar='25'#'45'#'TAR'#'pt18'#
# figure(num=None, figsize=(12, 8))
#
# plt.subplot(211)
# x = np.loadtxt(ref+'_'+tar+'.prf')
# plt.plot(x,)
# plt.title('Smash++')
# plt.grid(linewidth=.3)
#
# plt.subplot(212)
# y = np.loadtxt(tar+'.iae')
# plt.plot(y)
# plt.title('GeCo')
# plt.grid(linewidth=.3)
# plt.show()


# ref='refs'#'HS18'#
# tar='tars'#''PT18'#
# # figure(num=None, figsize=(12, 8))
#
# y = np.loadtxt(ref+'-'+tar+'.fil')
# x = np.arange(len(y))
# thresh = 1 #0.844905
# plt.fill_between(x, y, thresh, where=y<=thresh, color='green')
# plt.plot(x, np.full(len(y), thresh), color='brown')
# plt.plot(y, color='green')
# # plt.title('Smash++')
# plt.xlabel('Bases')
# plt.ylabel('Average entropy (bps)')
# plt.grid(linewidth=.3)
# plt.show()


figure(num=None, figsize=(10, 10))
plt.subplot(411)
thresh = 1.0
y = np.loadtxt('refs-tars.fil128')
x = np.arange(len(y))
plt.fill_between(x, y, thresh, where=y<=thresh, color='green')
plt.plot(x, np.full(len(y), thresh), color='brown')
plt.plot(y, color='green', label='Window=128')
plt.xlabel('Bases')
plt.ylabel('Average entropy (bps)')
plt.grid(linewidth=.3)
plt.legend()

plt.subplot(412)
y = np.loadtxt('refs-tars.fil256')
plt.fill_between(x, y, thresh, where=y<=thresh, color='green')
plt.plot(x, np.full(len(y), thresh), color='brown')
plt.plot(y, color='green', label='Window=256')
# plt.xlabel('Bases')
plt.ylabel('Average entropy (bps)')
plt.grid(linewidth=.3)
plt.legend()

plt.subplot(413)
y = np.loadtxt('refs-tars.fil512')
plt.fill_between(x, y, thresh, where=y<=thresh, color='green')
plt.plot(x, np.full(len(y), thresh), color='brown')
plt.plot(y, color='green', label='Window=512')
# plt.xlabel('Bases')
plt.ylabel('Average entropy (bps)')
plt.grid(linewidth=.3)
plt.legend()

plt.subplot(414)
y = np.loadtxt('refs-tars.fil1024')
plt.fill_between(x, y, thresh, where=y<=thresh, color='green')
plt.plot(x, np.full(len(y), thresh), color='brown')
plt.plot(y, color='green', label='Window=1024')
plt.xlabel('Bases')
plt.ylabel('Average entropy (bps)')
plt.grid(linewidth=.3)
plt.legend()
plt.show()
# plt.savefig('Smash++.jpg')
#
#
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