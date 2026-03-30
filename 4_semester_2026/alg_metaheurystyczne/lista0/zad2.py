import matplotlib.pyplot as plt
import os

with open("wyniki", "r") as f:
    lines = f.readlines()
    nazwa, average10, average50, MIN = [], [], [], []
    for line in lines:
        data = line.split()
        nazwa.append(data[0])
        average10.append(float(data[1]))
        average50.append(float(data[2]))
        MIN.append(float(data[3]))

for i in range(len(nazwa)):
    plt.figure(figsize=(8, 5))
    etykiety = ['Średnia minimum grup 10', 'Średnia minimum grup 50', 'Minimum']
    wartosci = [average10[i], average50[i], MIN[i]]
    bars = plt.bar(etykiety, wartosci)
    for bar, val in zip(bars, wartosci):
        plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(),
                 f'{val:.2f}', ha='center', va='bottom', fontsize=10)
    plt.xlabel('Metryka')
    plt.ylabel('Wartość')
    plt.title(f'Wyniki dla {nazwa[i]}')

plt.show()