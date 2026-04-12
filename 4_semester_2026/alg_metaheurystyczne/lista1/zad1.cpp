#include <bits/stdc++.h>
using namespace std;

struct LSResult {
    vector<int> path;
    double cost; // Zostawiamy double w ostatecznym wyniku dla czytelności z plikiem
    int steps;
};

// Wczytywanie plików
vector<pair<double, double>> readFiles(const string& nazwaPliku) {
    vector<pair<double, double>> coordinates;
    ifstream plik(nazwaPliku);
    if(!plik.is_open()){
        cout << "Blad przy otwieraniu pliku: " << nazwaPliku << "\n";
        return coordinates;
    }
    string linia;
    while (getline(plik, linia)) {
        if (!linia.empty() && linia.back() == '\r') linia.pop_back();
        if (linia == "NODE_COORD_SECTION") break;
    }
    
    while (getline(plik, linia)) {
        if (!linia.empty() && linia.back() == '\r') linia.pop_back();
        if (linia == "EOF" || linia.empty()) break;

        stringstream ss(linia);
        int id;
        double x, y;
        ss >> id >> x >> y;
        coordinates.push_back({x, y});
    }
    return coordinates;
}

// ZOPTYMALIZOWANA MACIERZ 1D NA TYPIE INT
vector<int> buildDistanceMatrix(const vector<pair<double, double>>& coords) {
    int n = coords.size();
    vector<int> distMatrix(n * n, 0); // Płaska tablica zamiast 2D
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                double dx = coords[i].first - coords[j].first;
                double dy = coords[i].second - coords[j].second;
                distMatrix[i * n + j] = round(sqrt(dx * dx + dy * dy)); 
            }
        }
    }
    return distMatrix;
}

// SZYBKIE LICZENIE KOSZTU (na podstawie macierzy 1D)
double calculateCost(const vector<int>& path, const vector<int>& distMatrix) {
    double total_cost = 0.0;
    int n = path.size();
    for (int i = 0; i < n - 1; ++i) {
        total_cost += distMatrix[path[i] * n + path[i+1]];
    }
    total_cost += distMatrix[path.back() * n + path.front()];
    return total_cost;
}

// SZYBKA DELTA Z UŻYCIEM INTÓW I MACIERZY 1D
inline int calculateDelta(const vector<int>& path, const vector<int>& distMatrix, int n, int i, int j) {
    if (i == 0 && j == n - 1) return 0;

    int przed_i = (i == 0) ? n - 1 : i - 1;
    int za_j = (j == n - 1) ? 0 : j + 1;

    int miasto_i = path[i];
    int miasto_j = path[j];
    int miasto_przed_i = path[przed_i];
    int miasto_za_j = path[za_j];

    int usuniete = distMatrix[miasto_przed_i * n + miasto_i] + distMatrix[miasto_j * n + miasto_za_j];
    int dodane = distMatrix[miasto_przed_i * n + miasto_j] + distMatrix[miasto_i * n + miasto_za_j];

    return dodane - usuniete;
}

// GŁÓWNY ALGORYTM (Dostosowany do nowej macierzy)
LSResult localSearch_Zadanie1(int n, const vector<int>& distMatrix) {
    vector<int> current_path(n);
    iota(current_path.begin(), current_path.end(), 0);

    random_device rd;
    mt19937 g(rd());
    shuffle(current_path.begin(), current_path.end(), g);

    double current_cost = calculateCost(current_path, distMatrix);
    int improvement_steps = 0;

    while (true) {
        int best_delta = 0; // Teraz szukamy inta mniejszego od 0
        int best_i = -1;
        int best_j = -1;

        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                int delta = calculateDelta(current_path, distMatrix, n, i, j);
                if (delta < best_delta) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        if (best_delta >= 0) break; // Zmieniono warunek ułamkowy na twarde zero 

        reverse(current_path.begin() + best_i, current_path.begin() + best_j + 1);
        current_cost += best_delta;
        improvement_steps++;
    }

    current_cost = calculateCost(current_path, distMatrix);
    return {current_path, current_cost, improvement_steps};
}

// ZAPIS WSPÓŁRZĘDNYCH DLA PYTHONA
void zapiszTraseDoPliku(const string& nazwaPliku, const vector<int>& trasa, const vector<pair<double, double>>& coords) {
    ofstream plik(nazwaPliku + "_trasa.txt");
    for (int idx : trasa) {
        plik << coords[idx].first << " " << coords[idx].second << "\n";
    }
    plik << coords[trasa[0]].first << " " << coords[trasa[0]].second << "\n";
    plik.close();
}

// KIEROWNIK ZADANIA
void uruchomZadanie1(const string& nazwaPliku, const vector<int>& distMatrix, const vector<pair<double, double>>& coords, ofstream& plik_wynikow) {
    int n = coords.size(); 
    int liczba_prob = 100; 
    
    double suma_kosztow = 0.0;
    long long suma_krokow = 0;
    double najlepszy_koszt = numeric_limits<double>::infinity();
    vector<int> najlepsza_trasa;

    cout << "Rozpoczynam obliczenia dla: " << nazwaPliku << " (n=" << n << ", prob=" << liczba_prob << ")...\n";

    for (int iteracja = 0; iteracja < liczba_prob; ++iteracja) {
        cout << "\rLiczenie: " << iteracja + 1 << " / " << liczba_prob 
             << " | Najlepszy koszt: " << (najlepszy_koszt == numeric_limits<double>::infinity() ? 0 : najlepszy_koszt);
        cout.flush();

        LSResult wynik = localSearch_Zadanie1(n, distMatrix);
        
        suma_kosztow += wynik.cost;
        suma_krokow += wynik.steps;
        
        if (wynik.cost < najlepszy_koszt) {
            najlepszy_koszt = wynik.cost;
            najlepsza_trasa = wynik.path;
        }
    }

    cout << "\r                                                                                \r";
    
    double sredni_koszt = suma_kosztow / liczba_prob;
    double srednie_kroki = static_cast<double>(suma_krokow) / liczba_prob;

    cout << "--- ZAKONCZONO " << nazwaPliku << " ---\n";
    cout << "1. Srednia wartosc: " << sredni_koszt << "\n";
    cout << "2. Srednia liczba krokow: " << srednie_kroki << "\n";
    cout << "3. Najlepszy koszt: " << najlepszy_koszt << "\n\n";

    plik_wynikow << "--- " << nazwaPliku << " ---\n";
    plik_wynikow << "Srednia wartosc: " << sredni_koszt << "\n";
    plik_wynikow << "Srednia liczba krokow: " << srednie_kroki << "\n";
    plik_wynikow << "Najlepszy koszt: " << najlepszy_koszt << "\n\n";

    zapiszTraseDoPliku(nazwaPliku, najlepsza_trasa, coords);
}

int main(){
    vector<string> pliki = {
        "ca4663.tsp", 
        "eg7146.tsp", 
        "ei8246.tsp", 
        "mu1979.tsp", 
        "tz6117.tsp"
    };

    ofstream plik_wynikow("wyniki_zbiorcze.txt");

    for (const string& plik_nazwa : pliki) {
        vector<pair<double, double>> coords = readFiles(plik_nazwa);
        
        if (coords.empty()) {
            cout << "Pominiecie pliku " << plik_nazwa << " (brak danych).\n\n";
            continue; 
        }
        
        // Zbudowanie zoptymalizowanej macierzy
        vector<int> matrix = buildDistanceMatrix(coords);
        uruchomZadanie1(plik_nazwa, matrix, coords, plik_wynikow);
    }

    plik_wynikow.close();
    cout << "Wszystkie obliczenia zakonczone. Zobacz 'wyniki_zbiorcze.txt' oraz pliki '_trasa.txt'.\n";

    return 0;
}