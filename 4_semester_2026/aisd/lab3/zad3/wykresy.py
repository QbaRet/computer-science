import pandas as pd
import matplotlib.pyplot as plt

try:
    df = pd.read_csv('wyniki_zad3.csv')
except FileNotFoundError:
    print("Nie znaleziono pliku 'wyniki_zad3.csv'.")
    exit()

group_sizes = [3, 5, 7, 9, 19, 21]

# Przypisujemy kolory dla kazdego rozmiaru grupy, zeby byly czytelne
colors = {
    3: 'red', 
    5: 'green', 
    7: 'blue', 
    9: 'orange', 
    19: 'purple', 
    21: 'brown'
}

# Funkcja pomocnicza do rysowania wykresow
def plot_metric(metric, title, ylabel, filename):
    plt.figure(figsize=(12, 8))
    for gs in group_sizes:
        subset = df[df['group_size'] == gs]
        # Dla rozmiaru 5 (klasycznego) dajemy nieco grubsza linie jako punkt odniesienia
        linewidth = 3 if gs == 5 else 1.5 
        plt.plot(subset['n'], subset[metric], 
                 label=f'Grupy po {gs}', 
                 color=colors[gs], 
                 linewidth=linewidth)

    plt.title(title, fontsize=16)
    plt.xlabel('Rozmiar danych (n)', fontsize=14)
    plt.ylabel(ylabel, fontsize=14)
    plt.legend(title="Rozmiar podziału", fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename)
    print(f"Zapisano wykres: {filename}")

# Generowanie trzech wykresów
plot_metric('avg_porownania', 'Liczba porównań w zależności od n i rozmiaru grup', 'Średnia liczba porównań', 'zad3_porownania.png')
plot_metric('avg_przestawienia', 'Liczba przestawień w zależności od n i rozmiaru grup', 'Średnia liczba przestawień', 'zad3_przestawienia.png')
plot_metric('avg_czas_us', 'Czas działania (mikrosekundy) w zależności od n i rozmiaru grup', 'Średni czas (µs)', 'zad3_czas.png')