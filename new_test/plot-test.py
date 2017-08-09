import numpy as np
import matplotlib.pyplot as plt

from matplotlib.ticker import NullFormatter  # useful for `logit` scale

# Fixing random state for reproducibility

# make up some data in the interval ]0, 1[

# values of hideo's perfomance test

testo_ascissa = "req/s"
testo_ordinata = "time(ms)"
#in x-axis a show req/s

#con 10 thread
#x_hideo = np.array([11.1, 100.9, 200.9, 400.7, 500.7, 1000.2])
#y_hideo_avg = np.array([0.5, 0.4, 0.5, 0.4, 0.4, 0.9])

#con 20 thread

x_hideo_20 = np.array([11.1, 100.9, 150.9, 200.9, 300.7, 400.6, 500.7, 800.6, 1000.3, 1500.5, 1999.1, 2305])
y_hideo_20 = np.array([0.3, 0.4, 0.3, 0.4, 0.3, 0.3, 0.2, 0.2, 0.3, 0.2, 0.3, 2.8])

x_apache = np.array([11.1, 101.0, 200.8, 400.9, 500.8, 1000.6, 1999.7, 2305])
y_apache_avg = np.array([0.4, 0.3, 0.2, 0.2, 0.2, 0.2, 0.3, 0.5])
#ax1 = fig.add_axes(x_hideo)
# in y


#ay1 = fig.add_axes(y_hideo_avg)

# linear
plt.xlabel(testo_ascissa)
plt.ylabel(testo_ordinata)

plt.plot(x_hideo_20, y_hideo_20, color = 'blue', marker = '*')
plt.plot(x_apache, y_apache_avg, color = 'red', marker = '.')

plt.title('average connection time')



plt.show()
