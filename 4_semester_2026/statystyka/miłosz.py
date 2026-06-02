import numpy as np
from scipy.stats import norm
import matplotlib.pyplot as plt

def test(mu, sig, n):
    b1=0

    for i in range(1000):
        x = np.random.normal(mu,sig,n)
        avg = np.mean(x)
        z = (avg-200)/(sig/np.sqrt(n))
        if abs(z) > 1.96:
            b1 += 1
    return b1/1000

def theoPow(mu,sig,n):
    delta = (mu-200)/(sig/np.sqrt(n))
    return norm.cdf(-1.96 - delta) + 1 - norm.cdf(1.96 - delta)

print(test(200,18,36))
print(test(202,18,36))
print(test(205,18,36))
print(test(210,18,36))
print(test(220,18,36))

pows = []
tpows = []
tpows9 = []
tpows100 = []
mu_vals = np.arange(185,216)
for mu in mu_vals:
    pows.append(test(mu, 18, 36))
    tpows.append(theoPow(mu,18,36))
    tpows100.append(theoPow(mu,18,100))
    tpows9.append(theoPow(mu,18,9))
    

plt.plot(mu_vals, pows, label = "Empirical power of tests")
plt.plot(mu_vals, tpows, label = "Theoretical power of tests")
plt.title("Theoretical vs empirical power of tests for mu[185;215], sigma=18, n=36")
plt.xlabel("Mu values")
plt.ylabel("Test power")
plt.legend()
plt.show()

plt.plot(mu_vals, tpows9, label = "Theoretical power of tests, n=9")
plt.plot(mu_vals, tpows, label = "Theoretical power of tests, n=36")
plt.plot(mu_vals, tpows100, label = "Theoretical power of tests, n=100")
plt.title("Theoretical power of tests for mu[185;215], sigma=18, n in {9,36,100}")
plt.xlabel("Mu values")
plt.ylabel("Test power")
plt.legend()
plt.show()