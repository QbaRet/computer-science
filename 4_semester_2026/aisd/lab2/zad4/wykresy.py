import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('wyniki_macierz.csv', sep=';')

plt.figure(figsize=(10, 6))

plt.plot(df['m'], df['staircase_comps'], label='Staircase (m+n)', color='red')
plt.plot(df['m'], df['binary_comps'], label='Binary (m log n)', color='blue')

plt.yscale('log')

plt.title('Porównanie liczby porównań')
plt.xlabel('Liczba wierszy (m)')
plt.ylabel('Liczba porównań (skala log)')
plt.legend()
plt.grid(True)

plt.show()