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
y_hideo_min = np.array([0.3, 0.2, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.2])

x_apache = np.array([10.1, 100.1, 151.0, 300.7, 800.5, 1000.6, 2303.9, 2716.4])
y_apache_min = np.array([0.3, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1])

plt.plot(x_hideo_20, y_hideo_min, color = 'blue', marker = '*', label='Hideo')
plt.plot(x_apache, y_apache_min, color = 'red', marker = '.', label='Apache2')

plt.title('min connection time')

plt.legend(loc=9)
plt.grid()

plt.savefig('test_min.png')
plt.show()
