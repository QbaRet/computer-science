import pandas as pd
import matplotlib.pyplot as plt
import io

try:
    with open('wyniki_zad4b.csv', 'r') as file:
        csv_data = file.read()
except FileNotFoundError:
    exit(1)

df = pd.read_csv(io.StringIO(csv_data))

plt.figure(figsize=(12, 6))

for algo in df['algorytm'].unique():
    subset = df[df['algorytm'] == algo]
    plt.plot(subset['n'], subset['porownania'], marker='.', linestyle='-', label=algo, alpha=0.8, linewidth=1.5)
plt.title('Porównanie liczby operacji: BinarySearch vs SmartSearch', fontsize=14)
plt.xlabel('Rozmiar danych (n)', fontsize=12)
plt.ylabel('Liczba porównań', fontsize=12)
plt.legend(title='Algorytm', fontsize=10)

plt.grid(True, linestyle='--', alpha=0.6)
plt.tight_layout()

plt.show()