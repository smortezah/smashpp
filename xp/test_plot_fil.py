import matplotlib.pyplot as plt

# name = 'X.seq.X.seq.fil'
name = '13.seq.1.seq.fil'

y0, y1 = [], []
for line0 in open('0.'+name, 'r'):
    values0 = [float(s) for s in line0.split()]
    y0.append(values0[0])
for line1 in open('1.'+name, 'r'):
    values1 = [float(s) for s in line1.split()]
    y1.append(values1[0])

f, (ax1, ax2) = plt.subplots(2, 1, sharey=True)
ax1.plot(y0)
ax1.set_title('ir=0')
ax2.plot(y1)
# ax1.set_title('ir=1')
