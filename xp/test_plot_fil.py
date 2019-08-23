import matplotlib.pyplot as plt

y0, y1 = [], []
# for line0 in open('0.gga24_1M.26.fil', 'r'):
# for line0 in open('0.24.26.fil', 'r'):
# for line0 in open('0.21.seq.23.seq.fil', 'r'):
# for line0 in open('0.19.seq.21.seq.fil', 'r'):
# for line0 in open('0.17.seq.19.seq.fil', 'r'):
# for line0 in open('0.18.seq.20.seq.fil', 'r'):
# for line0 in open('0.15.seq.17.seq.fil', 'r'):
# for line0 in open('0.13.seq.15.seq.fil', 'r'):
# for line0 in open('0.1.seq.1.seq.fil', 'r'):
# for line0 in open('0.2.seq.2.seq.fil', 'r'):
# for line0 in open('0.4.seq.4.seq.fil', 'r'):
# for line0 in open('0.ref.tar.prf', 'r'):
# for line0 in open('0.gga24_1M.26.fil', 'r'):
for line0 in open('0.PXO99A.seq.MAFF_311018.seq.fil', 'r'):
    values0 = [float(s) for s in line0.split()]
    y0.append(values0[0])
# for line1 in open('1.gga24_1M.26.fil', 'r'):
# for line1 in open('1.24.26.fil', 'r'):
# for line1 in open('1.21.seq.23.seq.fil', 'r'):
# for line1 in open('1.19.seq.21.seq.fil', 'r'):
# for line1 in open('1.17.seq.19.seq.fil', 'r'):
# for line1 in open('1.18.seq.20.seq.fil', 'r'):
# for line1 in open('1.15.seq.17.seq.fil', 'r'):
# for line1 in open('1.13.seq.15.seq.fil', 'r'):
# for line1 in open('1.1.seq.1.seq.fil', 'r'):
# for line1 in open('1.2.seq.2.seq.fil', 'r'):
# for line1 in open('1.4.seq.4.seq.fil', 'r'):
# for line1 in open('1.ref.tar.prf', 'r'):
# for line1 in open('1.gga24_1M.26.fil', 'r'):
for line1 in open('1.PXO99A.seq.MAFF_311018.seq.fil', 'r'):
    values1 = [float(s) for s in line1.split()]
    y1.append(values1[0])

f, (ax1, ax2) = plt.subplots(2, 1, sharey=True)
ax1.plot(y0)
ax1.set_title('ir=0')
ax2.plot(y1)
# ax1.set_title('ir=1')
