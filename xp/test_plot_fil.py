import matplotlib.pyplot as plt

y0, y1 = [], []
for line0 in open('0.28.seq.30.seq.fil', 'r'):
  values0 = [float(s) for s in line0.split()]
  y0.append(values0[0])
for line1 in open('1.28.seq.30.seq.fil', 'r'):
  values1 = [float(s) for s in line1.split()]
  y1.append(values1[0])

f, (ax1, ax2) = plt.subplots(1, 2, sharey=True)
ax1.plot(y0)
ax1.set_title('ir=0')
ax2.plot(y1)
# ax1.set_title('ir=1')