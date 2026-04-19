import matplotlib.pyplot as plt


def formatuj_nazwe_panstwa(nazwa):
    nazwa_czysta = nazwa.replace(" (linia)", "").strip()
    if nazwa_czysta.lower().endswith('.tsp'):
        nazwa_czysta = nazwa_czysta[:-4]
    return nazwa_czysta

def wczytaj_i_narysuj_trasy(nazwa_pliku):
    datasets = {}
    
    aktualny_dataset_stary = None
    wspolrzedne_stare = []

    try:
        with open(nazwa_pliku, 'r') as plik:
            for linia in plik:
                linia = linia.strip()
                
                if not linia:
                    continue

                if linia == 'END':
                    if aktualny_dataset_stary and wspolrzedne_stare:
                        datasets[aktualny_dataset_stary] = wspolrzedne_stare
                        wspolrzedne_stare = []
                    aktualny_dataset_stary = None
                    continue

                if linia.startswith('DATASET:'):
                    if aktualny_dataset_stary and wspolrzedne_stare:
                        datasets[aktualny_dataset_stary] = wspolrzedne_stare
                        wspolrzedne_stare = []
                    aktualny_dataset_stary = linia.split(':')[1].strip()
                    continue
                
                if aktualny_dataset_stary is not None:
                    dane = linia.split()
                    if len(dane) == 2:
                        try:
                            x = float(dane[0])
                            y = float(dane[1])
                            wspolrzedne_stare.append((x, y))
                        except ValueError:
                            pass 
                    continue 
                if ',' in linia and '|' in linia:
                    elementy = linia.split(',')
                    if len(elementy) < 2:
                        continue
                        
                    nazwa_nowy = elementy[0].strip()
                    ciag_trasy = elementy[-1].strip()
                    
                    wspolrzedne_nowe = []
                    punkty = ciag_trasy.split('|')
                    
                    for punkt in punkty:
                        dane_punktu = punkt.split(':')
                        if len(dane_punktu) >= 3: 
                            try:
                                x = float(dane_punktu[1])
                                y = float(dane_punktu[2])
                                wspolrzedne_nowe.append((x, y))
                            except ValueError:
                                pass 
                                
                    if nazwa_nowy and wspolrzedne_nowe:
                        if nazwa_nowy in datasets:
                            nazwa_nowy += " (linia)"
                        datasets[nazwa_nowy] = wspolrzedne_nowe

        if aktualny_dataset_stary and wspolrzedne_stare:
            datasets[aktualny_dataset_stary] = wspolrzedne_stare

    except FileNotFoundError:
        return

    for nazwa, coords in datasets.items():
        if not coords:
            continue
            
        x = [p[0] for p in coords]
        y = [p[1] for p in coords]
        if coords[0] != coords[-1]:
            x.append(coords[0][0])
            y.append(coords[0][1])

        plt.figure(figsize=(12, 8))
        plt.plot(x, y, linestyle='-', linewidth=0.8, color='blue', marker='o', markersize=1, markerfacecolor='red')
        
        plt.plot(x[0], y[0], marker='o', markersize=6, color='green', label='Start/Koniec')

        plt.title(formatuj_nazwe_panstwa(nazwa), fontsize=14, fontweight='bold')
        plt.xlabel("Oś X")
        plt.ylabel("Oś Y")
        plt.grid(True, linestyle='--', alpha=0.6)
        plt.legend()

        plt.tight_layout()
        plt.show()

if __name__ == "__main__":
    wczytaj_i_narysuj_trasy('trasy_z3.txt')