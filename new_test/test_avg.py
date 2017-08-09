import numpy as np
import matplotlib.pyplot as plt

from matplotlib.ticker import NullFormatter  # useful for `logit` scale

# values of hideo's perfomance test

testo_ascissa = "req/s"
testo_ordinata = "time(ms)"
#in x-axis a show req/s

#con 10 thread
#x_hideo = np.array([11.1, 100.9, 200.9, 400.7, 500.7, 1000.2])
#y_hideo_avg = np.array([0.5, 0.4, 0.5, 0.4, 0.4, 0.9])

#con 20 thread
x_hideo_20 = np.array([11.1, 100.9, 150.9, 200.9, 300.7, 400.6, 500.7, 800.6, 1000.3, 1500.5, 1999.1, 2305.3])
y_hideo_20 = np.array([0.3, 0.4, 0.3, 0.4, 0.3, 0.3, 0.2, 0.2, 0.3, 0.2, 0.3, 2.8])

x_apache = np.array([10.1, 101.0, 151.0, 200.8, 300.7, 400.9, 500.8, 800, 1000.1, 1500.4, 1999.7, 2305.3])
y_apache_avg = np.array([0.4, 0.2, 0.3, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.3, 0.3, 0.5])

# linear
plt.xlabel(testo_ascissa)
plt.ylabel(testo_ordinata)

plt.plot(x_hideo_20, y_hideo_20, color = 'blue', marker = '*', label='Hideo')
plt.plot(x_apache, y_apache_avg, color = 'red', marker = '.', label='Apache2')

plt.title('average connection time')
#legend centered
plt.legend(loc=9)
plt.grid()

plt.savefig('test_avg.png')
plt.show()
