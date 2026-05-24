#include <bits/stdc++.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct SAResult {
    vector<int> path;
    double cost;
    int steps;
};

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


SAResult simulatedAnnealing(int n, const vector<int>& distMatrix, double T_start, double alpha, int inner_loops, mt19937& g) {
    vector<int> current_path(n);
    iota(current_path.begin(), current_path.end(), 0);
    shuffle(current_path.begin(), current_path.end(), g);
    
    double current_cost = calculateCost(current_path, distMatrix);
    
    vector<int> best_path = current_path;
    double best_cost = current_cost;
    int accepted_steps = 0;
    
    double T = T_start;
    double T_end = 0.001;
    
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


int main(){
    vector<string> pliki = {"dj38.tsp", "wi29.tsp", "qa194.tsp", "uy734.tsp", "zi929.tsp"};

    vector<double> T_starts = {1000.0, 10000.0};
    vector<double> alphas = {0.90, 0.95, 0.99, 0.995};
    vector<int> multi_inner_loops = {1, 2, 5};

    int prob_na_kombinacje = 5;

    ofstream plik_raportu("raport_parametrow_SA.txt");
    if (!plik_raportu.is_open()) {
        cerr << "Blad: nie mozna otworzyc pliku raportu.\n";
        return 1;
    }

    plik_raportu << "Plik;T_start;Alpha;Mnoznik_Petli;Sredni_Koszt;Najlepszy_Koszt;Czas_Srednio_ms\n";

    random_device rd;
    mt19937 g(rd());

    cout << "Rozpoczynam testowanie parametrow...\n";

    for (const string& plik_nazwa : pliki) {
        cout << "Testowanie pliku: " << plik_nazwa << "\n";

        vector<pair<double, double>> coords = readFiles(plik_nazwa);
        if(coords.empty()) {
            cout << " - blad odczytu pliku\n";
            continue;
        }

        vector<int> matrix = buildDistanceMatrix(coords);
        int n = coords.size();

        for(double T0 : T_starts) {
            for(double alpha : alphas) {
                for(int mult : multi_inner_loops) {

                    int inner_loops = n * mult;

                    double suma_kosztow = 0;
                    double najlepszy_koszt = numeric_limits<double>::infinity();
                    long long calkowity_czas_ms = 0;

                    cout << "Test: T0=" << T0 << ", alpha=" << alpha << ", wew_petle=n*" << mult << " -> ";

                    for(int i = 0; i < prob_na_kombinacje; ++i) {
                        auto start_time = high_resolution_clock::now();

                        SAResult wynik = simulatedAnnealing(n, matrix, T0, alpha, inner_loops, g);

                        auto end_time = high_resolution_clock::now();
                        calkowity_czas_ms += duration_cast<milliseconds>(end_time - start_time).count();

                        suma_kosztow += wynik.cost;
                        if(wynik.cost < najlepszy_koszt) najlepszy_koszt = wynik.cost;
                    }

                    double sredni_koszt = suma_kosztow / prob_na_kombinacje;
                    double sredni_czas = (double)calkowity_czas_ms / prob_na_kombinacje;

                    cout << "Sr. Koszt: " << sredni_koszt << " (Czas: " << sredni_czas << " ms)\n";

                    plik_raportu << plik_nazwa << ";"
                                 << T0 << ";"
                                 << alpha << ";"
                                 << mult << ";"
                                 << fixed << setprecision(2) << sredni_koszt << ";"
                                 << najlepszy_koszt << ";"
                                 << setprecision(1) << sredni_czas << "\n";
                    plik_raportu.flush();
                }
            }
        }
    }

    cout << "\nZakonczono! Raport zostal zapisany w pliku: raport_parametrow_SA.txt\n";
    return 0;
}