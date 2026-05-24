import matplotlib.pyplot as plt
import os


def odbij_wzgledem_oy(coords):
    if not coords:
        return []

    return [(-x, y) for x, y in coords]


def obroc_w_prawo(coords):
    if not coords:
        return []

    sr_x = sum(p[0] for p in coords) / len(coords)
    sr_y = sum(p[1] for p in coords) / len(coords)

    obrocone = []
    for x, y in coords:
        przes_x = x - sr_x
        przes_y = y - sr_y
        nowy_x = sr_x + przes_y
        nowy_y = sr_y - przes_x
        obrocone.append((nowy_x, nowy_y))

    return obrocone

def wczytaj_i_narysuj_trasy(nazwa_pliku, folder_wyjsciowy="wykresy"):
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
        print(f"Błąd: Nie znaleziono pliku '{nazwa_pliku}'.")
        return

    if not os.path.exists(folder_wyjsciowy):
        os.makedirs(folder_wyjsciowy)

    for nazwa, coords in datasets.items():
        if not coords:
            continue

        coords = obroc_w_prawo(odbij_wzgledem_oy(coords))
            
        x = [p[0] for p in coords]
        y = [p[1] for p in coords]

        plt.figure(figsize=(10, 10)) 
        
        plt.plot(x, y, linestyle='-', linewidth=1.0, color='royalblue', alpha=0.8, zorder=1)
        
        rozmiar_punktu = 4 if len(coords) < 500 else 1
        plt.scatter(x, y, color='red', s=rozmiar_punktu, zorder=2)
        
        plt.plot(x[0], y[0], marker='o', markersize=8, color='limegreen', label='Start/Koniec', zorder=3)

        plt.title(f"Trasa {nazwa}", fontsize=14, fontweight='bold')
        plt.xlabel("Współrzędna X")
        plt.ylabel("Współrzędna Y")
        plt.grid(True, linestyle='--', alpha=0.5)
        plt.legend()
        plt.tight_layout()

        nazwa_pliku_wyj = os.path.join(folder_wyjsciowy, f"trasa_{nazwa}.png")
        plt.savefig(nazwa_pliku_wyj, dpi=300) 
        plt.close()
        
        print(f"Wygenerowano i zapisano wykres: {nazwa_pliku_wyj}")

if __name__ == "__main__":
    nazwa_twojego_pliku = 'trasy_TS_MT_20260521_152552.txt' 
    
    wczytaj_i_narysuj_trasy(nazwa_twojego_pliku, folder_wyjsciowy="wykresy_TabuSearch")