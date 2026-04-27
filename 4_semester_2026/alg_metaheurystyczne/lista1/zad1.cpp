#include <bits/stdc++.h>
using namespace std;

struct LSResult {
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

LSResult localSearch_Zadanie1(int n, const vector<int>& distMatrix) {
    vector<int> current_path(n);
    iota(current_path.begin(), current_path.end(), 0);
    
    random_device rd;
    mt19937 g(rd());
    shuffle(current_path.begin(), current_path.end(), g);
    
    double current_cost = calculateCost(current_path, distMatrix);
    int improvement_steps = 0;
    
    while (true) {
        int best_delta = 0;
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
        
        if (best_delta >= 0) break; 
        
        reverse(current_path.begin() + best_i, current_path.begin() + best_j + 1);
        current_cost += best_delta;
        improvement_steps++;
    }
    
    return {current_path, current_cost, improvement_steps};
}
void zapiszTrasy(ofstream& plik, const string& nazwa, const vector<int>& trasa, const vector<pair<double, double>>& coords) {
    plik << "DATASET: " << nazwa << "\n";
    for (int idx : trasa) {
        plik << coords[idx].first << " " << coords[idx].second << "\n";
    }
    plik << coords[trasa[0]].first << " " << coords[trasa[0]].second << "\n";
}

int main(){
    vector<string> pliki = {"mu1979.tsp", "tz6117.tsp", "dj38.tsp", "qa194.tsp", "uy734.tsp", "wi29.tsp", "zi929.tsp","ei8246.tsp"};
    ofstream plik_wynikow("wyniki_zbiorcze_z1.txt");
    ofstream plik_tras("trasy_z11.txt");
    
    for (const string& plik_nazwa : pliki) {
        cout << "[Z1] Plik: " << plik_nazwa << flush;
        vector<pair<double, double>> coords = readFiles(plik_nazwa);
        if(coords.empty()) {
            cout << " - pominiecie\n";
            continue;
        }
        
        vector<int> matrix = buildDistanceMatrix(coords);
        int n = coords.size();
        int liczba_prob = (n < 1000) ? n : 100;
        
        double suma_kosztow = 0;
        long long suma_krokow = 0;
        double najlepszy_koszt = numeric_limits<double>::infinity();
        vector<int> najlepsza_trasa;
        
        for (int i = 0; i < liczba_prob; ++i) {
            LSResult wynik = localSearch_Zadanie1(n, matrix);
            
            suma_kosztow += wynik.cost;
            suma_krokow += wynik.steps;
            
            if (wynik.cost < najlepszy_koszt) {
                najlepszy_koszt = wynik.cost;
                najlepsza_trasa = wynik.path;
            }
        }
        
        plik_wynikow << plik_nazwa << " Srednia: " << suma_kosztow/liczba_prob << " Kroki: " << (double)suma_krokow/liczba_prob << " Best: " << najlepszy_koszt << "\n";
        

        zapiszTrasy(plik_tras, plik_nazwa, najlepsza_trasa, coords);
    }
    return 0;
}