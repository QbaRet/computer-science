import matplotlib.pyplot as plt
import os

def wczytaj_i_narysuj_trasy(nazwa_pliku, folder_wyjsciowy="wykresy"):
    datasets = {}
    aktualny_dataset = None
    wspolrzedne = []

    # 1. Odczyt danych z pliku wygenerowanego przez C++
    try:
        with open(nazwa_pliku, 'r') as plik:
            for linia in plik:
                linia = linia.strip()
                
                if not linia:
                    continue

                if linia == 'END':
                    if aktualny_dataset and wspolrzedne:
                        datasets[aktualny_dataset] = wspolrzedne
                        wspolrzedne = []
                    aktualny_dataset = None
                    continue

                if linia.startswith('DATASET:'):
                    if aktualny_dataset and wspolrzedne:
                        datasets[aktualny_dataset] = wspolrzedne
                        wspolrzedne = []
                    aktualny_dataset = linia.split(':')[1].strip()
                
                else:
                    if aktualny_dataset is not None:
                        dane = linia.split()
                        if len(dane) == 2:
                            try:
                                x = float(dane[0])
                                y = float(dane[1])
                                wspolrzedne.append((x, y))
                            except ValueError:
                                pass 
            
            # Zapisz ostatni dataset (gdyby brakowało słowa END na samym końcu pliku)
            if aktualny_dataset and wspolrzedne:
                datasets[aktualny_dataset] = wspolrzedne

    except FileNotFoundError:
        print(f"Błąd: Nie znaleziono pliku '{nazwa_pliku}'.")
        return

    # Upewnij się, że folder wyjściowy istnieje
    if not os.path.exists(folder_wyjsciowy):
        os.makedirs(folder_wyjsciowy)

    # 2. Generowanie wykresów
    for nazwa, coords in datasets.items():
        if not coords:
            continue
            
        # POPRAWKA: x to p[0], a y to p[1]
        x = [p[0] for p in coords]
        y = [p[1] for p in coords]

        plt.figure(figsize=(10, 10)) # Kwadratowy wykres czesto wyglada lepiej na mapach
        
        # Rysowanie głównej trasy (lekko przezroczysta linia dla czytelności na dużych grafach)
        plt.plot(x, y, linestyle='-', linewidth=1.0, color='royalblue', alpha=0.8, zorder=1)
        
        # Rysowanie punktów miast (dla map > 1000 wierzchołków lepiej zmniejszyć markersize)
        rozmiar_punktu = 4 if len(coords) < 500 else 1
        plt.scatter(x, y, color='red', s=rozmiar_punktu, zorder=2)
        
        # Wyróżnienie punktu startowego/końcowego
        plt.plot(x[0], y[0], marker='o', markersize=8, color='limegreen', label='Start/Koniec', zorder=3)

        # Upiększanie wykresu
        plt.title(f"Trasa TSP: {nazwa}\nLiczba miast: {len(coords)-1}", fontsize=14, fontweight='bold')
        plt.xlabel("Współrzędna X")
        plt.ylabel("Współrzędna Y")
        plt.grid(True, linestyle='--', alpha=0.5)
        plt.legend()
        plt.tight_layout()

        # Zapisywanie do pliku zamiast plt.show()
        nazwa_pliku_wyj = os.path.join(folder_wyjsciowy, f"trasa_{nazwa}.png")
        plt.savefig(nazwa_pliku_wyj, dpi=300) # dpi=300 gwarantuje super jakość do druku/PDF
        plt.close() # Zamknięcie figury uwalnia pamięć
        
        print(f"Wygenerowano i zapisano wykres: {nazwa_pliku_wyj}")

if __name__ == "__main__":
    # Tutaj wstawiasz nazwę pliku wygenerowanego przez Twój program C++
    # Np. skopiuj nazwę wygenerowaną przez znacznik czasu z konsoli
    nazwa_twojego_pliku = 'trasy_SA_MT_20260521_135937.txt' 
    
    # Skrypt stworzy folder "wykresy_TabuSearch" i wrzuci tam wszystkie 10 obrazków
    wczytaj_i_narysuj_trasy(nazwa_twojego_pliku, folder_wyjsciowy="wykresy_SymulowaneWyzarzanie")