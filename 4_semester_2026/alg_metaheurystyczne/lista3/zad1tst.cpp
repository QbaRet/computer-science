#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

// --- STRUKTURY DANYCH ---
struct Individual {
    vector<int> path;
    double cost;
    bool operator<(const Individual& other) const {
        return cost < other.cost;
    }
};

struct GAResult {
    vector<int> path;
    double cost;
};

enum class CrossoverMethod { OX, PMX };

struct Config {
    CrossoverMethod cx_method;
    int island_pop_size;
    double mut_rate;
    double memetic_prob;
    int tournament_size;
    int migration_interval;
};

struct TestResult {
    Config conf;
    double avg_cost;
    double best_cost;
};

// --- WCZYTYWANIE DANYCH ---
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
        int id; double x, y;
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

// --- OPERATORY GENETYCZNE ---
void mutateInversion(vector<int>& path, double mut_rate, mt19937& g) {
    uniform_real_distribution<double> dist_prob(0.0, 1.0);
    if (dist_prob(g) < mut_rate) {
        uniform_int_distribution<int> dist_node(0, path.size() - 1);
        int i = dist_node(g);
        int j = dist_node(g);
        if (i > j) swap(i, j);
        reverse(path.begin() + i, path.begin() + j + 1);
    }
}

vector<int> crossoverOX(const vector<int>& p1, const vector<int>& p2, mt19937& g) {
    int n = p1.size();
    vector<int> child(n, -1);
    uniform_int_distribution<int> dist(0, n - 1);
    int start = dist(g), end = dist(g);
    if (start > end) swap(start, end);

    vector<bool> in_child(n, false);
    for (int i = start; i <= end; ++i) {
        child[i] = p1[i];
        in_child[p1[i]] = true;
    }

    int curr_p2 = 0;
    for (int i = 0; i < n; ++i) {
        if (i >= start && i <= end) continue;
        while (in_child[p2[curr_p2]]) curr_p2++;
        child[i] = p2[curr_p2];
        in_child[p2[curr_p2]] = true;
    }
    return child;
}

vector<int> crossoverPMX(const vector<int>& p1, const vector<int>& p2, mt19937& g) {
    int n = p1.size();
    vector<int> child(n, -1);
    uniform_int_distribution<int> dist(0, n - 1);
    int start = dist(g), end = dist(g);
    if (start > end) swap(start, end);

    for(int i = start; i <= end; ++i) child[i] = p1[i];

    for(int i = 0; i < n; ++i) {
        if(i >= start && i <= end) continue;
        int candidate = p2[i];
        bool mapping_needed = true;
        while(mapping_needed) {
            mapping_needed = false;
            for(int j = start; j <= end; ++j) {
                if(candidate == p1[j]) {
                    candidate = p2[j];
                    mapping_needed = true;
                    break;
                }
            }
        }
        child[i] = candidate;
    }
    return child;
}

const Individual& tournamentSelection(const vector<Individual>& pop, int tournament_size, mt19937& g) {
    uniform_int_distribution<int> dist(0, pop.size() - 1);
    int best_idx = dist(g);
    for (int i = 1; i < tournament_size; ++i) {
        int idx = dist(g);
        if (pop[idx].cost < pop[best_idx].cost) {
            best_idx = idx;
        }
    }
    return pop[best_idx];
}

void memeticLocalSearch(vector<int>& path, double& cost, const vector<int>& distMatrix, int n, mt19937& g) {
    uniform_int_distribution<int> dist_node(0, n - 1);
    int limit_prob = n; // Zalezne od rozmiaru grafu
    for (int k = 0; k < limit_prob; ++k) {
        int i = dist_node(g);
        int j = dist_node(g);
        if (i > j) swap(i, j);
        if (i == j || (i == 0 && j == n - 1)) continue;
        
        int delta = calculateDelta(path, distMatrix, n, i, j);
        if (delta < 0) {
            reverse(path.begin() + i, path.begin() + j + 1);
            cost += delta;
        }
    }
}

// --- ALGORYTM WYSPOWY ---
GAResult islandGeneticAlgorithm(int n, const vector<int>& distMatrix, int num_islands, int generations, Config conf, mt19937& g) {
    vector<vector<Individual>> islands(num_islands, vector<Individual>(conf.island_pop_size));
    vector<int> base_path(n);
    iota(base_path.begin(), base_path.end(), 0);
    
    for (int i = 0; i < num_islands; ++i) {
        for (int j = 0; j < conf.island_pop_size; ++j) {
            islands[i][j].path = base_path;
            shuffle(islands[i][j].path.begin(), islands[i][j].path.end(), g);
            islands[i][j].cost = calculateCost(islands[i][j].path, distMatrix);
        }
    }
    
    Individual best_overall = islands[0][0];
    int migrants_count = max(1, conf.island_pop_size / 10); 
    int epochs = generations / conf.migration_interval;
    if (epochs == 0) epochs = 1;

    uniform_real_distribution<double> d_prob(0.0, 1.0);

    for (int epoch = 0; epoch < epochs; ++epoch) {
        for (int isl = 0; isl < num_islands; ++isl) {
            for (int gen = 0; gen < conf.migration_interval; ++gen) {
                vector<Individual> new_pop(conf.island_pop_size);
                auto island_best = *min_element(islands[isl].begin(), islands[isl].end());
                new_pop[0] = island_best;
                if (island_best.cost < best_overall.cost) best_overall = island_best;

                for (int i = 1; i < conf.island_pop_size; ++i) {
                    const Individual& p1 = tournamentSelection(islands[isl], conf.tournament_size, g);
                    const Individual& p2 = tournamentSelection(islands[isl], conf.tournament_size, g);
                    
                    vector<int> child;
                    if (conf.cx_method == CrossoverMethod::OX) {
                        child = crossoverOX(p1.path, p2.path, g);
                    } else {
                        child = crossoverPMX(p1.path, p2.path, g);
                    }
                    
                    mutateInversion(child, conf.mut_rate, g);
                    double c_cost = calculateCost(child, distMatrix);

                    if (conf.memetic_prob > 0.0 && d_prob(g) < conf.memetic_prob) {
                        memeticLocalSearch(child, c_cost, distMatrix, n, g);
                    }
                    new_pop[i] = {child, c_cost};
                }
                islands[isl] = move(new_pop);
            }
        }

        if (epoch < epochs - 1) {
            vector<vector<Individual>> migrants(num_islands);
            for (int isl = 0; isl < num_islands; ++isl) {
                sort(islands[isl].begin(), islands[isl].end()); 
                for(int m = 0; m < migrants_count; ++m) {
                    migrants[isl].push_back(islands[isl][m]); 
                }
            }
            for (int isl = 0; isl < num_islands; ++isl) {
                int target_isl = (isl + 1) % num_islands; 
                for(int m = 0; m < migrants_count; ++m) {
                    islands[target_isl][conf.island_pop_size - 1 - m] = migrants[isl][m];
                }
            }
        }
    }
    
    for (int isl = 0; isl < num_islands; ++isl) {
        auto island_best = *min_element(islands[isl].begin(), islands[isl].end());
        if (island_best.cost < best_overall.cost) best_overall = island_best;
    }
    return {best_overall.path, best_overall.cost};
}

// --- GŁÓWNA PĘTLA KALIBRACYJNA DLA WIELU PLIKÓW ---
int main() {
    // Lista plików do przetestowania (zakomentuj większe pliki, jeśli testy idą zbyt wolno)
    const vector<string> pliki = {
        "wi29.tsp", "dj38.tsp", "qa194.tsp", "uy734.tsp", "zi929.tsp"
    };

    const int num_islands = 4;
    const int generations = 500; 
    const int total_trials = 10; 
    const int num_threads = thread::hardware_concurrency(); 

    // ROZBUDOWANA SIATKA: (2x2x3x3x2x2 = 144 kombinacje na plik)
    vector<CrossoverMethod> cx_methods = {CrossoverMethod::OX, CrossoverMethod::PMX};
    vector<int> pop_sizes = {20, 50};
    vector<double> mut_rates = {0.05, 0.15, 0.3};
    vector<double> memetic_probs = {0.05, 0.15, 0.3}; 
    vector<int> tournament_sizes = {3, 7};
    vector<int> migration_intervals = {25, 50};

    int total_configs = cx_methods.size() * pop_sizes.size() * mut_rates.size() * memetic_probs.size() * tournament_sizes.size() * migration_intervals.size();
    
    ofstream plik_raport("raport_kalibracji_wieloplikowy.txt");
    plik_raport << "=== RAPORT KALIBRACJI ZAAWANSOWANEJ DLA WIELU ZBIOROW ===\n\n";

    for(const string& plik_nazwa : pliki) {
        vector<pair<double, double>> coords = readFiles(plik_nazwa);
        if(coords.empty()) {
            cout << "\n[!] Blad wczytywania: " << plik_nazwa << "\n";
            continue;
        }

        vector<int> matrix = buildDistanceMatrix(coords);
        int n = coords.size();
        
        cout << "\n==========================================================\n";
        cout << " ROZPOCZYNAM KALIBRACJE DLA: " << plik_nazwa << " (Miast: " << n << ")\n";
        cout << " Laczna liczba konfiguracji: " << total_configs << "\n";
        cout << "==========================================================\n";
        
        vector<TestResult> wszystkie_wyniki;
        int current_config_num = 1;

        for(auto cx : cx_methods) {
            for(int p_size : pop_sizes) {
                for(double m_rate : mut_rates) {
                    for(double mem_prob : memetic_probs) {
                        for(int t_size : tournament_sizes) {
                            for(int m_int : migration_intervals) {
                                
                                Config obecny_conf = {cx, p_size, m_rate, mem_prob, t_size, m_int};
                                
                                atomic<int> current_trial(0);
                                mutex mtx;
                                double suma_kosztow = 0.0;
                                double najlepszy_z_prob = numeric_limits<double>::infinity();

                                auto worker = [&](int thread_id) {
                                    random_device rd;
                                    mt19937 g(rd() ^ (thread_id * 1999) ^ time(nullptr));

                                    while (true) {
                                        int trial_idx = current_trial.fetch_add(1);
                                        if (trial_idx >= total_trials) break;

                                        GAResult wynik = islandGeneticAlgorithm(n, matrix, num_islands, generations, obecny_conf, g);

                                        lock_guard<mutex> lock(mtx);
                                        suma_kosztow += wynik.cost;
                                        if (wynik.cost < najlepszy_z_prob) najlepszy_z_prob = wynik.cost;
                                    }
                                };

                                vector<thread> threads;
                                for (unsigned int i = 0; i < num_threads; ++i) threads.emplace_back(worker, i);
                                for (auto& t : threads) t.join();

                                double sredni_koszt = suma_kosztow / total_trials;
                                wszystkie_wyniki.push_back({obecny_conf, sredni_koszt, najlepszy_z_prob});

                                string method_str = (cx == CrossoverMethod::OX) ? "OX " : "PMX";
                                cout << " [" << plik_nazwa << "] Postep: " << current_config_num++ << "/" << total_configs 
                                     << " | Srednia: " << fixed << setprecision(2) << sredni_koszt << " \r" << flush;
                            }
                        }
                    }
                }
            }
        }

        cout << "\n\n -> TOP 5 NAJLEPSZYCH KONFIGURACJI DLA " << plik_nazwa << ":\n";
        plik_raport << "WYNIKI DLA ZBIORU: " << plik_nazwa << " (N=" << n << ")\n";
        
        sort(wszystkie_wyniki.begin(), wszystkie_wyniki.end(), [](const TestResult& a, const TestResult& b) {
            if(abs(a.avg_cost - b.avg_cost) > 0.001) return a.avg_cost < b.avg_cost;
            return a.best_cost < b.best_cost;
        });
        
        for(int i = 0; i < min(5, (int)wszystkie_wyniki.size()); ++i) {
            auto res = wszystkie_wyniki[i];
            string method_str = (res.conf.cx_method == CrossoverMethod::OX) ? "OX" : "PMX";
            
            cout << "  " << i+1 << ". Srednia: " << res.avg_cost << " | Najlepszy strzal: " << res.best_cost << "\n"
                 << "     Krzyzowanie: " << method_str << " | Pop. wyspy: " << res.conf.island_pop_size 
                 << " | Mut: " << res.conf.mut_rate << " | Memetyka: " << res.conf.memetic_prob 
                 << " | Turniej: " << res.conf.tournament_size << " | Migracja: " << res.conf.migration_interval << "\n\n";
                 
            plik_raport << i+1 << ". Srednia: " << res.avg_cost << " (Najlepszy: " << res.best_cost << ")\n"
                        << "   - Metoda: " << method_str << "\n"
                        << "   - Rozmiar wyspy: " << res.conf.island_pop_size << "\n"
                        << "   - Mutacja: " << res.conf.mut_rate << "\n"
                        << "   - Prawd. Memetyczne: " << res.conf.memetic_prob << "\n"
                        << "   - Rozmiar turnieju: " << res.conf.tournament_size << "\n"
                        << "   - Interwal migracji: " << res.conf.migration_interval << "\n"
                        << "-----------------------------------------\n";
        }
        plik_raport << "\n==========================================================\n\n";
    }

    plik_raport.close();
    cout << "\nGotowe! Najlepsze konfiguracje zostaly zapisane w pliku 'raport_kalibracji_wieloplikowy.txt'.\n";

    return 0;
}