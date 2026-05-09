#include <bits/stdc++.h>

using namespace std;

struct SAResult {
    vector<int> path;
    double cost; 
    int steps;
};

// Funkcja wczytująca pliki TSPLIB
vector<pair<double, double>> readFiles(const string& nazwaPliku) {
    vector<pair<double, double>> coordinates;
    ifstream plik(nazwaPliku);
    if(!plik.is_open()) return coordinates;
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

// Funkcja budująca macierz odległości
vector<int> buildDistanceMatrix(const vector<pair<double, double>>& coords) {
    int n = coords.size();
    vector<int> distMatrix(n * n, 0); 
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

// Funkcja licząca początkowy koszt
double calculateCost(const vector<int>& path, const vector<int>& distMatrix) {
    double total_cost = 0.0;
    int n = path.size();
    for (int i = 0; i < n - 1; ++i) {
        total_cost += distMatrix[path[i] * n + path[i+1]];
    }
    total_cost += distMatrix[path.back() * n + path.front()];
    return total_cost;
}

// Szybkie liczenie delty dla 2-opt
inline int calculateDelta(const vector<int>& path, const vector<int>& distMatrix, int n, int i, int j) {
    if (i == 0 && j == n - 1) return 0;
    int przed_i = (i == 0) ? n - 1 : i - 1;
    int za_j = (j == n - 1) ? 0 : j + 1;
    
    int usuniete = distMatrix[path[przed_i] * n + path[i]] + distMatrix[path[j] * n + path[za_j]];
    int dodane = distMatrix[path[przed_i] * n + path[j]] + distMatrix[path[i] * n + path[za_j]];
    
    return dodane - usuniete;
}

// Algorytm Symulowanego Wyżarzania
SAResult simulatedAnnealing(int n, const vector<int>& distMatrix, double T_start, double T_end, double alpha, int inner_loops, mt19937& g) {
    vector<int> current_path(n);
    iota(current_path.begin(), current_path.end(), 0);
    shuffle(current_path.begin(), current_path.end(), g);
    
    double current_cost = calculateCost(current_path, distMatrix);
    
    vector<int> best_path = current_path;
    double best_cost = current_cost;
    int accepted_steps = 0;
    
    double T = T_start;
    uniform_int_distribution<int> dist_node(0, n - 1);
    uniform_real_distribution<double> dist_prob(0.0, 1.0);
    
    while (T > T_end) {
        for (int k = 0; k < inner_loops; ++k) {
            int i = dist_node(g);
            int j = dist_node(g);
            if (i > j) swap(i, j);
            if (i == j || (i == 0 && j == n - 1)) continue;
            
            int delta = calculateDelta(current_path, distMatrix, n, i, j);
            
            if (delta < 0 || dist_prob(g) < exp(-delta / T)) {
                reverse(current_path.begin() + i, current_path.begin() + j + 1);
                current_cost += delta;
                accepted_steps++;
                
                if (current_cost < best_cost) {
                    best_cost = current_cost;
                    best_path = current_path;
                }
            }
        }
        T *= alpha;
    }
    
    return {best_path, best_cost, accepted_steps};
}

// Funkcja zapisująca trasy - wywołuje na końcu flush()
void zapiszTrasy(ofstream& plik, const string& nazwa, const vector<int>& trasa, const vector<pair<double, double>>& coords) {
    plik << "DATASET: " << nazwa << "\n";
    for (int idx : trasa) {
        plik << coords[idx].first << " " << coords[idx].second << "\n";
    }
    plik << coords[trasa[0]].first << " " << coords[trasa[0]].second << "\n";
    plik << "END\n";
    plik.flush();
}

// Tworzenie unikalnego znacznika dla plików wyjściowych
string makeRunStamp() {
    time_t now = time(nullptr);
    tm local_time{};
    // Używamy localtime_s pod Windows lub localtime_r pod Linux. 
    // W czystym standardzie C++ można też użyć std::localtime, ale dla bezpieczeństwa polecam:
#if defined(_WIN32)
    localtime_s(&local_time, &now);
#else
    localtime_r(&now, &local_time);
#endif
    ostringstream stamp;
    stamp << put_time(&local_time, "%Y%m%d_%H%M%S");
    return stamp.str();
}

int main(){
    const vector<string> pliki = {
        "dj38.tsp", "qa194.tsp", "wi29.tsp", "uy734.tsp", "zi929.tsp", 
        "ca4663.tsp", "eg7146.tsp", "ei8246.tsp", "mu1979.tsp", "tz6117.tsp"
    };
    
    const string run_stamp = makeRunStamp();
    const string nazwa_wynikow = "wyniki_zbiorcze_SA_" + run_stamp + ".txt";
    const string nazwa_tras = "trasy_SA_" + run_stamp + ".txt";
    
    ofstream plik_wynikow(nazwa_wynikow);
    ofstream plik_tras(nazwa_tras);
    
    if (!plik_wynikow.is_open() || !plik_tras.is_open()) {
        cerr << "Blad: nie mozna otworzyc plikow wyjsciowych.\n";
        return 1;
    }

    // Nagłówek pliku wynikowego
    time_t now = time(nullptr);
    plik_wynikow << "--- nowa sesja: " << ctime(&now);
    plik_wynikow.flush();

    // Globalny generator liczb losowych
    random_device rd;
    mt19937 g(rd());

    for (const string& plik_nazwa : pliki) {
        cout << "\n========================================\n";
        cout << "[SA] Plik: " << plik_nazwa << "\n";
        
        vector<pair<double, double>> coords = readFiles(plik_nazwa);
        if(coords.empty()) {
            cout << " - pominiecie (blad odczytu pliku)\n";
            plik_wynikow << plik_nazwa << "; blad: nie udalo sie wczytac danych\n";
            plik_wynikow.flush();
            continue;
        }
        
        vector<int> matrix = buildDistanceMatrix(coords);
        int n = coords.size();
        
        // --- Parametry wg polecenia 1 ---
        double T_start = 10000.0;
        double T_end = 0.001;
        double alpha = 0.995; 
        int inner_loops = n * 2; 
        int liczba_prob;
        
        if (n < 1000) {
            cout << "Rozmiar (" << n << ") < 1000. Faza testowania parametrow.\n";
            liczba_prob = 10; // Zmień na potrzebną wartość do testów
        } else {
            cout << "Rozmiar (" << n << ") >= 1000. Duze dane - wymuszone 100 prob.\n";
            liczba_prob = 100; 
        }

        double suma_kosztow = 0;
        double najlepszy_koszt = numeric_limits<double>::infinity();
        vector<int> najlepsza_trasa;
        
        cout << "Rozpoczecie obliczen (" << liczba_prob << " prob)...\n";
        for (int i = 0; i < liczba_prob; ++i) {
            SAResult wynik = simulatedAnnealing(n, matrix, T_start, T_end, alpha, inner_loops, g);
            suma_kosztow += wynik.cost;
            
            if (wynik.cost < najlepszy_koszt) {
                najlepszy_koszt = wynik.cost;
                najlepsza_trasa = wynik.path;
            }
            
            // Zapisz na bieżąco do pliku po każdej próbie
            plik_wynikow << plik_nazwa 
                         << "; proba " << i + 1 << "/" << liczba_prob
                         << "; koszt: " << wynik.cost
                         << "; zaakceptowane kroki: " << wynik.steps << "\n";
            plik_wynikow.flush(); // WYMUSZENIE ZAPISU
            
            // Pasek postępu na konsoli (żeby było widać, że program żyje)
            if ((i + 1) % 10 == 0 || i == liczba_prob - 1) {
                cout << " Skonczono: " << i + 1 << "/" << liczba_prob << "\r" << flush;
            }
        }
        cout << "\n";
        
        // Podsumowanie pojedynczego pliku z polecenia "podaj najlepsze (...) oraz srednia"
        double srednia_wartosc = suma_kosztow / liczba_prob;
        cout << "Najlepsze uzyskane rozwiazanie: " << najlepszy_koszt << "\n";
        cout << "Srednia wartosc uzyskiwanych rozwiazan: " << srednia_wartosc << "\n";
        
        // Dodatkowy wpis podsumowujący do pliku
        plik_wynikow << "-> PODSUMOWANIE " << plik_nazwa 
                     << " Najlepsze: " << najlepszy_koszt 
                     << " Srednia: " << srednia_wartosc << "\n\n";
        plik_wynikow.flush();
                     
        // Zapis najlepszej znalezionej trasy
        zapiszTrasy(plik_tras, plik_nazwa, najlepsza_trasa, coords);
    }
    
    cout << "\n========================================\n";
    cout << "Zadanie z SA zakonczone w calosci.\n";
    return 0;
}