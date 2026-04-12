import numpy as np
import scipy.stats as stats
import matplotlib.pyplot as plt

def moc_empiryczna(mi):
    mi_zero = 200
    sigma = 18
    alfa = 0.05
    n = 36
    B = 1000
    X_bar = np.mean(np.random.normal(loc=mi, scale=sigma, size=(B, n)), axis=1)
    Z = (X_bar - mi_zero) / (sigma / np.sqrt(n))
    Z_alfa = stats.norm.ppf(1 - alfa / 2)
    odrzucenia = np.abs(Z) > Z_alfa
    odp = np.sum(odrzucenia)
    procent_odrzucenia = odp / B * 100
    return odp, procent_odrzucenia

def moc_teoretyczna(mi, n):
    mi_zero = 200
    sigma = 18
    alfa = 0.05
    delta = (mi - mi_zero) / (sigma / np.sqrt(n))
    moc = 1 - stats.norm.cdf(stats.norm.ppf(1 - alfa / 2) - delta) + stats.norm.cdf(-stats.norm.ppf(1 - alfa / 2) - delta)
    return moc

mi_values = [200, 202, 205, 210, 220]
for i in mi_values:
    odp, procent_odrzucenia = moc_empiryczna(i)
    print(f"mi={i}: Odrzuceń={odp}, Procent odrzuceń={procent_odrzucenia:.2f}%")

mi_zakres = np.linspace(185, 215, 100)
moc_teoretyczna_values = [moc_teoretyczna(mi, 36) for mi in mi_zakres]
moc_empiryczna_values = [moc_empiryczna(mi)[0] / 1000 for mi in mi_zakres]

plt.figure()
plt.plot(mi_zakres, moc_teoretyczna_values, label='Teoretyczna', color='blue')
plt.plot(mi_zakres, moc_empiryczna_values, label='Empiryczna', color='orange', linestyle='--') 

plt.axvline(200, color='red', linestyle=':', label='mi0=200 (H0)')
plt.title('Porównanie krzywych mocy testu (n=36)')
plt.xlabel('Prawdziwe opóźnienie mi')
plt.ylabel('Moc testu')
plt.legend() 
plt.grid()
plt.show()

plt.figure()
n_values = [9, 36, 100]
for n in n_values:
    moc_teoretyczna_values_n = [moc_teoretyczna(mi, n) for mi in mi_zakres]
    plt.plot(mi_zakres, moc_teoretyczna_values_n, label=f'n={n}')

plt.axvline(200, color='red', linestyle=':', label='mi0=200 (H0)')
plt.title('Krzywe mocy testu dla różnych wielkości próby n')
plt.xlabel('Prawdziwe opóźnienie mi')
plt.ylabel('Moc testu')
plt.legend()
plt.grid()
plt.show()