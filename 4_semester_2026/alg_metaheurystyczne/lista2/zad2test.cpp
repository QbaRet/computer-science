#include <bits/stdc++.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct TSResult {
    vector<int> path;
    double cost; 
};

// =========================================================================
// FUNKCJE POMOCNICZE (Wczytywanie, Macierz, Koszt, Delta) - bez zmian
// =========================================================================

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

double calculateCost(const vector<int>& path, const vector<int>& distMatrix) {
    double total_cost = 0.0;
    int n = path.size();
    for (int i = 0; i < n - 1; ++i) {
        total_cost += distMatrix[path[i] * n + path[i+1]];
    }
    total_cost += distMatrix[path.back() * n + path.front()];
    return total_cost;
}

inline int calculateDelta(const vector<int>& path, const vector<int>& distMatrix, int n, int i, int j) {
    if (i == 0 && j == n - 1) return 0;
    int przed_i = (i == 0) ? n - 1 : i - 1;
    int za_j = (j == n - 1) ? 0 : j + 1;
    
    int usuniete = distMatrix[path[przed_i] * n + path[i]] + distMatrix[path[j] * n + path[za_j]];
    int dodane = distMatrix[path[przed_i] * n + path[j]] + distMatrix[path[i] * n + path[za_j]];
    
    return dodane - usuniete;
}

// =========================================================================
// ALGORYTM TABU SEARCH
// =========================================================================

TSResult tabuSearch(int n, const vector<int>& distMatrix, int tabu_tenure, int max_iterations, int neighbors_sample_size, mt19937& g) {
    // 1. Inicjalizacja rozwiązania początkowego
    vector<int> current_path(n);
    iota(current_path.begin(), current_path.end(), 0);
    shuffle(current_path.begin(), current_path.end(), g);
    
    double current_cost = calculateCost(current_path, distMatrix);
    
    vector<int> best_path = current_path;
    double best_cost = current_cost;
    
    // 2. Inicjalizacja Listy Tabu (Macierz NxN przechowująca iterację, w której ruch wygasa)
    vector<vector<int>> tabu_list(n, vector<int>(n, 0));
    
    uniform_int_distribution<int> dist_node(0, n - 1);
    
    // 3. Główna pętla
    for (int iter = 1; iter <= max_iterations; ++iter) {
        int best_delta = numeric_limits<int>::max();
        int best_i = -1, best_j = -1;
        int best_city1 = -1, best_city2 = -1;
        
        // 4. Przeszukiwanie sąsiedztwa (wybieramy losową próbkę, by było szybko)
        for (int k = 0; k < neighbors_sample_size; ++k) {
            int i = dist_node(g);
            int j = dist_node(g);
            if (i > j) swap(i, j);
            if (i == j || (i == 0 && j == n - 1)) continue;
            
            int delta = calculateDelta(current_path, distMatrix, n, i, j);
            
            // Rejestrujemy, jakie miasta zamieniamy (krawędzie krawędziowe)
            int city1 = current_path[i];
            int city2 = current_path[j];
            
            // Sprawdzanie czy ruch jest na liście Tabu
            bool is_tabu = (tabu_list[city1][city2] >= iter || tabu_list[city2][city1] >= iter);
            
            // Kryterium aspiracji (Aspiration Criterion): 
            // Nawet jeśli ruch jest zakazany, akceptujemy go, o ile bije GLOBALNY REKORD.
            bool aspiration = (current_cost + delta < best_cost);
            
            // Jeśli ruch jest dozwolony LUB spełnia aspirację -> bierzemy najlepszy
            if (!is_tabu || aspiration) {
                if (delta < best_delta) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                    best_city1 = city1;
                    best_city2 = city2;
                }
            }
        }
        
        // 5. Zastosowanie najlepszego ruchu z sąsiedztwa (nawet jeśli pogarsza wynik!)
        if (best_i != -1) {
            reverse(current_path.begin() + best_i, current_path.begin() + best_j + 1);
            current_cost += best_delta;
            
            // Wpisanie ruchu na listę zakazanych
            tabu_list[best_city1][best_city2] = iter + tabu_tenure;
            tabu_list[best_city2][best_city1] = iter + tabu_tenure;
            
            // 6. Aktualizacja globalnego optimum
            if (current_cost < best_cost) {
                best_cost = current_cost;
                best_path = current_path;
            }
        }
    }
    
    return {best_path, best_cost};
}

// =========================================================================
// GŁÓWNY PROGRAM TESTUJĄCY
// =========================================================================

int main(){
    // Te same małe pliki co w SA, abyś mógł potem w sprawozdaniu zgrabnie to porównać
    vector<string> pliki = {"dj38.tsp", "wi29.tsp", "qa194.tsp", "uy734.tsp", "zi929.tsp"}; 
    
    // Siatka parametrów do przetestowania
    vector<int> tabu_tenures = {10, 30, 50}; // Jak długo ruch jest zakazany
    vector<int> mult_iterations = {10, 50};  // Mnożnik liczby iteracji (n * mult)
    vector<int> mult_neighbors = {2, 10};    // Ile losowych sąsiadów sprawdzamy w iteracji (n * mult)
    
    int prob_na_kombinacje = 5; 
    
    ofstream plik_raportu("raport_parametrow_TS.txt");
    if (!plik_raportu.is_open()) {
        cerr << "Blad: nie mozna otworzyc pliku raportu.\n";
        return 1;
    }
    
    plik_raportu << "Plik;Tabu_Tenure;Max_Iter_Mnoznik;Siedzi_Mnoznik;Sredni_Koszt;Najlepszy_Koszt;Czas_Srednio_ms\n";

    random_device rd;
    mt19937 g(rd());

    cout << "Rozpoczynam testowanie parametrow Tabu Search...\n";

    for (const string& plik_nazwa : pliki) {
        cout << "\n========================================\n";
        cout << "Testowanie pliku: " << plik_nazwa << "\n";
        
        vector<pair<double, double>> coords = readFiles(plik_nazwa);
        if(coords.empty()) continue;
        
        vector<int> matrix = buildDistanceMatrix(coords);
        int n = coords.size();
        
        for(int t_tenure : tabu_tenures) {
            for(int m_iter : mult_iterations) {
                for(int m_neigh : mult_neighbors) {
                    
                    int max_iter = n * m_iter;
                    int neigh_sample = n * m_neigh;
                    
                    double suma_kosztow = 0;
                    double najlepszy_koszt = numeric_limits<double>::infinity();
                    long long calkowity_czas_ms = 0;
                    
                    cout << "Test: Tabu=" << t_tenure << ", Iter=n*" << m_iter << ", Sasiedzi=n*" << m_neigh << " -> ";
                    
                    for(int i = 0; i < prob_na_kombinacje; ++i) {
                        auto start_time = high_resolution_clock::now();
                        
                        TSResult wynik = tabuSearch(n, matrix, t_tenure, max_iter, neigh_sample, g);
                        
                        auto end_time = high_resolution_clock::now();
                        calkowity_czas_ms += duration_cast<milliseconds>(end_time - start_time).count();
                        
                        suma_kosztow += wynik.cost;
                        if(wynik.cost < najlepszy_koszt) najlepszy_koszt = wynik.cost;
                    }
                    
                    double sredni_koszt = suma_kosztow / prob_na_kombinacje;
                    double sredni_czas = (double)calkowity_czas_ms / prob_na_kombinacje;
                    
                    cout << "Sr. Koszt: " << sredni_koszt << " (Czas: " << sredni_czas << " ms)\n";
                    
                    plik_raportu << plik_nazwa << ";" 
                                 << t_tenure << ";" 
                                 << m_iter << ";" 
                                 << m_neigh << ";" 
                                 << fixed << setprecision(2) << sredni_koszt << ";" 
                                 << najlepszy_koszt << ";" 
                                 << setprecision(1) << sredni_czas << "\n";
                    plik_raportu.flush();
                }
            }
        }
    }
    
    cout << "\nZakonczono! Raport zostal zapisany w pliku: raport_parametrow_TS.txt\n";
    return 0;
}