import numpy as np
import matplotlib.pyplot as plt

from matplotlib.ticker import NullFormatter  # useful for `logit` scale

# values of hideo's perfomance test

testo_ascissa = "req/s"
testo_ordinata = "time(ms)"
#in x-axis a show req/s

# linear
plt.xlabel(testo_ascissa)
plt.ylabel(testo_ordinata)

x_hideo_20 = np.array([11.1, 100.9, 150.9, 200.9, 300.7, 400.6, 500.7, 800.6, 1000.3, 1500.5, 1999.1, 2305.3, 2730.0])
y_hideo_max = np.array([0.5, 10.2,   1.7,   9.5,   3.7,   10.5,   3.9,  5.6,    1.8,   5.3,    8.4,    1035,   1234.8])

x_apache = np.array([10.1, 100.1, 151.0, 300.7, 800.5, 1000.6, 1500.4, 2000,  2303.9, 2716.4])
y_apache_max = np.array([2.1, 3.0, 8.2,   1.6,   4.8,   5.3,    2.0,   30.5,  6.0,    1046.4])

plt.plot(x_hideo_20, y_hideo_max, color = 'blue', marker = '*', label='Hideo')
plt.plot(x_apache, y_apache_max, color = 'red', marker = '.', label='Apache2')

plt.title('max connection time')

plt.legend(loc=9)
plt.grid()

plt.savefig('test_max.png')
plt.show()
