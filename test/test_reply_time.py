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


x_hideo = np.array([11.1, 100.9, 150.9, 200.9, 300.7, 400.6, 500.7, 800.6, 1000.3, 1500.5, 1999.1, 2305.3, 2337.3, 2730.0]) #conns/s
y_hideo = np.array([0.3,   0.4,   0.3,   0.4,   0.2,   0.3,   0.2,   0.2,    0.3,   0.2,    0.2,     0.3,   0.6,    10.7])   #reply time

x_apache = np.array([10.1, 100.1, 151.0, 300.7, 800.5, 1000.6, 1500.4, 2000.0,  2303.9, 2716.4]) #conns/s
y_apache = np.array([0.4,  0.2,    0.3,   0.2,   0.2,   0.2,    0.3,    0.7,    0.3,    13.5]) #reply time

plt.plot(x_hideo, y_hideo, color = 'blue', marker = '*', label='Hideo')
plt.plot(x_apache, y_apache, color = 'red', marker = '.', label='Apache2')

plt.title('Reply time')

plt.legend(loc=9)
plt.grid()

plt.savefig('test_reply.png')
plt.show()
