import matplotlib.pyplot as plt

def wczytaj_i_narysuj_trasy(nazwa_pliku):
    datasets = {}
    aktualny_dataset = None
    wspolrzedne = []

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
            if aktualny_dataset and wspolrzedne:
                datasets[aktualny_dataset] = wspolrzedne

    except FileNotFoundError:
        print(f"Błąd: Nie znaleziono pliku '{nazwa_pliku}'. Upewnij się, że jest w tym samym folderze co skrypt.")
        return

    for nazwa, coords in datasets.items():
        if not coords:
            continue
            
        x = [p[1] for p in coords]
        y = [p[0] for p in coords]

        if coords[0] != coords[-1]:
            x.append(coords[0][0])
            y.append(coords[0][1])

        plt.figure(figsize=(12, 8))
        plt.plot(x, y, linestyle='-', linewidth=0.8, color='blue', marker='o', markersize=1, markerfacecolor='red')
        
        plt.plot(x[0], y[0], marker='o', markersize=6, color='green', label='Start/Koniec')

        plt.title(f"Trasa Komiwojażera: {nazwa}", fontsize=14, fontweight='bold')
        plt.xlabel("Oś X")
        plt.ylabel("Oś Y")
        plt.grid(True, linestyle='--', alpha=0.6)
        plt.legend()

        plt.tight_layout()
        plt.show()

if __name__ == "__main__":
    wczytaj_i_narysuj_trasy('trasy_z2.txt')