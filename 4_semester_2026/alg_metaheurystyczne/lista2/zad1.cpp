#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

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

void zapiszTrasy(const string& nazwaPlikuTras, const string& nazwaZbioru, const vector<int>& trasa, const vector<pair<double, double>>& coords) {
    ofstream plik(nazwaPlikuTras, ios::app);
    plik << "DATASET: " << nazwaZbioru << "\n";
    for (int idx : trasa) {
        plik << coords[idx].first << " " << coords[idx].second << "\n";
    }
    plik << coords[trasa[0]].first << " " << coords[trasa[0]].second << "\n";
    plik << "END\n";
}

string makeRunStamp() {
    time_t now = time(nullptr);
    tm local_time{};
#if defined(_WIN32)
    localtime_s(&local_time, &now);
#else
    localtime_r(&now, &local_time);
#endif
    ostringstream stamp;
    stamp << put_time(&local_time, "%Y%m%d_%H%M%S");
    return stamp.str();
}


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


int main() {
    const vector<string> pliki = {
        "dj38.tsp", "qa194.tsp", "wi29.tsp", "uy734.tsp", "zi929.tsp",
        "ca4663.tsp", "eg7146.tsp", "ei8246.tsp", "mu1979.tsp", "tz6117.tsp"
    };

    const int num_threads = 8;

    const double T_start = 1000.0;
    const double T_end = 0.001;
    const double alpha = 0.99;

    const string run_stamp = makeRunStamp();
    const string nazwa_wynikow = "wyniki_zbiorcze_SA_MT_" + run_stamp + ".txt";
    const string nazwa_tras = "trasy_SA_MT_" + run_stamp + ".txt";

    {
        ofstream plik_wynikow(nazwa_wynikow);
        ofstream plik_tras(nazwa_tras);
        if (!plik_wynikow.is_open() || !plik_tras.is_open()) {
            return 1;
        }
        time_t now = time(nullptr);
    }

    for (const string& plik_nazwa : pliki) {

        vector<pair<double, double>> coords = readFiles(plik_nazwa);
        if(coords.empty()) {
            ofstream plik_wynikow(nazwa_wynikow, ios::app);
            plik_wynikow << plik_nazwa << "; blad wczytywania\n";
            continue;
        }

        vector<int> matrix = buildDistanceMatrix(coords);
        int n = coords.size();
        int inner_loops = n * 2;

        int total_trials;
        if (n < 1000) {
            total_trials = n;
        } else {
            total_trials = 100;
        }

        atomic<int> current_trial(0);
        mutex file_mtx;

        double najlepszy_koszt_ogolem = numeric_limits<double>::infinity();
        vector<int> najlepsza_trasa_ogolem;
        double suma_kosztow = 0.0;

        auto worker = [&](int thread_id) {
            random_device rd;
            mt19937 g(rd() ^ (thread_id * 1999) ^ time(nullptr));

            while (true) {
                int trial_idx = current_trial.fetch_add(1);
                if (trial_idx >= total_trials) {
                    break;
                }

                SAResult wynik = simulatedAnnealing(n, matrix, T_start, T_end, alpha, inner_loops, g);

                lock_guard<mutex> lock(file_mtx);

                suma_kosztow += wynik.cost;
                if (wynik.cost < najlepszy_koszt_ogolem) {
                    najlepszy_koszt_ogolem = wynik.cost;
                    najlepsza_trasa_ogolem = wynik.path;
                }

                ofstream plik_wynikow(nazwa_wynikow, ios::app);
                plik_wynikow << plik_nazwa
                             << "; proba " << trial_idx + 1 << "/" << total_trials
                             << "; koszt: " << wynik.cost << "\n";

                if ((trial_idx + 1) % 10 == 0 || trial_idx == total_trials - 1) {
                    cout << " Ukonczono lacznie: " << trial_idx + 1 << "/" << total_trials << "\r" << flush;
                }
            }
        };

        vector<thread> threads;
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(worker, i);
        }
        for (auto& t : threads) {
            t.join();
        }

        cout << "\n";

        double srednia = suma_kosztow / total_trials;
        cout << " Najlepsze rozwiazanie: " << najlepszy_koszt_ogolem << "\n";
        cout << " Srednia wartosc: " << srednia << "\n";

        {
            ofstream plik_wynikow(nazwa_wynikow, ios::app);
            plik_wynikow << "PODSUMOWANIE " << plik_nazwa
                         << " Najlepsze: " << najlepszy_koszt_ogolem
                         << " Srednia: " << srednia << "\n\n";
        }

        zapiszTrasy(nazwa_tras, plik_nazwa, najlepsza_trasa_ogolem, coords);
    }

    return 0;
}