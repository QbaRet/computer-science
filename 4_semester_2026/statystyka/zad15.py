import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

np.random.seed(42)  
proba = np.random.normal(700, 100, 100)

def oblicz_statystyki(dane, opis):
    s = pd.Series(dane)
    stats = {
        'Średnia': s.mean(),
        'Odchylenie (s)': s.std(),
        'Mediana': s.median(),
        'Q1': s.quantile(0.25),
        'Q3': s.quantile(0.75),
        'IQR': s.quantile(0.75) - s.quantile(0.25)
    }
    print(f"\nStatystyki: {opis}")
    for k, v in stats.items():
        print(f"{k}: {v:.2f}")
    return stats

oblicz_statystyki(proba, "Próba pierwsza")

fig, axs = plt.subplots(2, 2, figsize=(12, 10))

axs[0, 0].hist(proba, bins=10, color='skyblue', edgecolor='black')
axs[0, 0].set_title('Histogram (10 bins)')

axs[0, 1].hist(proba, bins=30, color='salmon', edgecolor='black')
axs[0, 1].set_title('Histogram (30 bins)')

x_sorted = np.sort(proba)
y_values = np.arange(1, len(x_sorted) + 1) / len(x_sorted)
axs[1, 0].step(x_sorted, y_values)
axs[1, 0].set_title('Empiryczna dystrybuanta')

axs[1, 1].boxplot(proba, vert=False)
axs[1, 1].set_title('Wykres pudełkowy')

plt.tight_layout()
plt.show()

outliery = np.array([1500] * 5)
nowa_proba = np.concatenate([proba, outliery])
oblicz_statystyki(nowa_proba, "Próba z outlierami (n=105)")