#include <bits/stdc++.h>
#include <sched.h>
#include <unistd.h>

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
                distMatrix[i * n + j] = (int)round(sqrt(dx * dx + dy * dy)); 
            }
        }
    }
    return distMatrix;
}

double calculateCost(const vector<int>& path, const vector<int>& distMatrix) {
    double total_cost = 0.0;
    int n = (int)path.size();
    for (int i = 0; i < n - 1; ++i) {
        total_cost += distMatrix[path[i] * n + path[i+1]];
    }
    total_cost += distMatrix[path.back() * n + path.front()];
    return total_cost;
}

inline int calculateDeltaTranspose(const vector<int>& path, const vector<int>& distMatrix, int n, int i, int j) {
    int przed_i = (i == 0) ? n - 1 : i - 1;
    int za_i = (i == n - 1) ? 0 : i + 1;
    int przed_j = (j == 0) ? n - 1 : j - 1;
    int za_j = (j == n - 1) ? 0 : j + 1;
    
    int usuniete, dodane;
    
    if (za_i == j) {
        usuniete = distMatrix[path[przed_i] * n + path[i]] + distMatrix[path[j] * n + path[za_j]];
        dodane = distMatrix[path[przed_i] * n + path[j]] + distMatrix[path[i] * n + path[za_j]];
    } else if (za_j == i) {
        usuniete = distMatrix[path[przed_j] * n + path[j]] + distMatrix[path[i] * n + path[za_i]];
        dodane = distMatrix[path[przed_j] * n + path[i]] + distMatrix[path[j] * n + path[za_i]];
    } else {
        usuniete = distMatrix[path[przed_i] * n + path[i]] + distMatrix[path[i] * n + path[za_i]] +
                   distMatrix[path[przed_j] * n + path[j]] + distMatrix[path[j] * n + path[za_j]];
        dodane = distMatrix[path[przed_i] * n + path[j]] + distMatrix[path[j] * n + path[za_i]] +
                 distMatrix[path[przed_j] * n + path[i]] + distMatrix[path[i] * n + path[za_j]];
    }
    
    return dodane - usuniete;
}

LSResult localSearch_Zadanie3(int n, const vector<int>& distMatrix, mt19937& g) {
    vector<int> current_path(n);
    iota(current_path.begin(), current_path.end(), 0);
    shuffle(current_path.begin(), current_path.end(), g);
    
    double current_cost = calculateCost(current_path, distMatrix);
    int improvement_steps = 0;
    
    while (true) {
        int best_delta = 0;
        int best_i = -1;
        int best_j = -1;
        
        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                int delta = calculateDeltaTranspose(current_path, distMatrix, n, i, j);
                if (delta < best_delta) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                }
            }
        }
        
        if (best_delta >= 0) break; 
        
        swap(current_path[best_i], current_path[best_j]);
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
    plik << "END\n";
    plik.flush();
}

string makeRunStamp() {
    time_t now = time(nullptr);
    tm local_time{};
    localtime_r(&now, &local_time);

    ostringstream stamp;
    stamp << put_time(&local_time, "%Y%m%d_%H%M%S") << "_pid" << getpid();
    return stamp.str();
}

void limitProcessToNCores(int cores) {
    cpu_set_t set;
    CPU_ZERO(&set);
    for (int i = 0; i < cores; ++i) CPU_SET(i, &set);


}

int main() {
    const vector<string> pliki_nazwy = {
        "ca4663.tsp", "dj38.tsp", "eg7146.tsp", "ei8246.tsp", "mu1979.tsp",
        "qa194.tsp", "tz6117.tsp", "uy734.tsp", "wi29.tsp", "zi929.tsp"
    };
    const int liczba_prob = 1;
    const int max_threads = 8;

    limitProcessToNCores(max_threads);

    const string run_stamp = makeRunStamp();
    const string nazwa_wynikow = "wyniki_zbiorcze_z3_" + run_stamp + ".txt";
    const string nazwa_tras = "trasy_z3_" + run_stamp + ".txt";

    ofstream plik_wynikow(nazwa_wynikow);
    ofstream plik_tras(nazwa_tras);

    if (!plik_wynikow.is_open() || !plik_tras.is_open()) {
        cerr << "Blad: nie mozna otworzyc plikow wyjsciowych.\n";
        return 1;
    }

    atomic<int> licznik_datasetu(0);
    mutex mtx;

    time_t now = time(nullptr);
    plik_wynikow << "datasety: " << pliki_nazwy[0];
    for (size_t i = 1; i < pliki_nazwy.size(); ++i) {
        plik_wynikow << ", " << pliki_nazwy[i];    plik_wynikow << "--- nowa sesja: " << ctime(&now);
    }
    plik_wynikow << "\n";

    auto worker = [&](int thread_id) {
        random_device rd;
        mt19937 g(rd() ^ (unsigned)thread_id ^ (unsigned)time(nullptr));
        
        while (true) {
            int idx = licznik_datasetu.fetch_add(1);
            if (idx >= (int)pliki_nazwy.size()) break;

            const string& plik_nazwa = pliki_nazwy[idx];
            vector<pair<double, double>> coords = readFiles(plik_nazwa);
            if (coords.empty()) {
                lock_guard<mutex> lock(mtx);
                plik_wynikow << plik_nazwa << "; blad: nie udalo sie wczytac danych\n";
                plik_wynikow.flush();
                continue;
            }

            vector<int> matrix = buildDistanceMatrix(coords);
            int n = (int)coords.size();
            LSResult res = localSearch_Zadanie3(n, matrix, g);

            lock_guard<mutex> lock(mtx);
            plik_wynikow << plik_nazwa << "; proba 1; koszt: " << res.cost
                         << "; kroki: " << res.steps << "\n";
            plik_wynikow.flush();

            zapiszTrasy(plik_tras, plik_nazwa, res.path, coords);
        }
    };

    vector<thread> threads;
    for (int i = 0; i < max_threads; ++i) {
        threads.emplace_back(worker, i);
    }
    for (auto& t : threads) t.join();

    return 0;
}