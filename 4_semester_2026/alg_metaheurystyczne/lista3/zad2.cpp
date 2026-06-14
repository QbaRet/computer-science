#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <csignal>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

struct City {
    int id;
    double x, y;
};

using Tour = std::vector<int>;

struct Individual {
    Tour tour;
    double fitness;
    double distance;
};

struct SimResult {
    std::string name;
    double bestDist;
    double avgDist;
    double worstDist;
    double timeSeconds;
    Tour bestTour;
    std::string crossoverMethod;
    bool memetic;
    bool island;
    int runs;
};


std::vector<City> loadTSP(const std::string& filename) {
    std::ifstream f(filename);
    if (!f.is_open()) throw std::runtime_error("Nie mozna otworzyc pliku: " + filename);

    std::vector<City> cities;
    std::string line;
    bool reading = false;

    while (std::getline(f, line)) {
        if (line.find("NODE_COORD_SECTION") != std::string::npos) { reading = true; continue; }
        if (line.find("EOF") != std::string::npos) break;
        if (!reading) continue;

        std::istringstream ss(line);
        City c;
        ss >> c.id >> c.x >> c.y;
        cities.push_back(c);
    }
    return cities;
}

bool isValidTour(const Tour& t, int n) {
    if ((int)t.size() != n) return false;
    std::vector<bool> seen(n, false);
    for (int c : t) {
        if (c < 0 || c >= n || seen[c]) return false;
        seen[c] = true;
    }
    return true;
}

std::vector<std::vector<double>> buildDistMatrix(const std::vector<City>& cities) {
    int n = (int)cities.size();
    std::vector<std::vector<double>> dist(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            double dx = cities[i].x - cities[j].x;
            double dy = cities[i].y - cities[j].y;
            dist[i][j] = std::sqrt(dx * dx + dy * dy);
        }
    return dist;
}

double tourDistance(const Tour& t, const std::vector<std::vector<double>>& dist) {
    double d = 0;
    int n = (int)t.size();
    for (int i = 0; i < n; ++i)
        d += dist[t[i]][t[(i + 1) % n]];
    return d;
}

Tour randomTour(int n, std::mt19937& rng) {
    Tour t(n);
    std::iota(t.begin(), t.end(), 0);
    std::shuffle(t.begin(), t.end(), rng);
    return t;
}

Tour greedyTour(int start, const std::vector<std::vector<double>>& dist) {
    int n = (int)dist.size();
    Tour t;
    t.reserve(n);
    std::vector<bool> visited(n, false);
    t.push_back(start);
    visited[start] = true;
    for (int step = 1; step < n; ++step) {
        int cur = t.back();
        double best = 1e18;
        int nxt = -1;
        for (int j = 0; j < n; ++j) {
            if (!visited[j] && dist[cur][j] < best) {
                best = dist[cur][j];
                nxt = j;
            }
        }
        t.push_back(nxt);
        visited[nxt] = true;
    }
    return t;
}

Tour twoOpt(Tour t, const std::vector<std::vector<double>>& dist,
            int maxIter = 500) {
    int n = (int)t.size();
    bool improved = true;
    int iter = 0;
    while (improved && iter < maxIter) {
        improved = false;
        ++iter;
        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 2; j < n; ++j) {
                if (i == 0 && j == n - 1) continue;
                double d0 = dist[t[i]][t[i + 1]] + dist[t[j]][t[(j + 1) % n]];
                double d1 = dist[t[i]][t[j]] + dist[t[i + 1]][t[(j + 1) % n]];
                if (d1 < d0 - 1e-10) {
                    std::reverse(t.begin() + i + 1, t.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
    return t;
}

Tour crossoverOX(const Tour& p1, const Tour& p2, std::mt19937& rng) {
    int n = (int)p1.size();
    std::uniform_int_distribution<int> dist(0, n - 1);
    int a = dist(rng), b = dist(rng);
    if (a > b) std::swap(a, b);

    Tour child(n, -1);
    std::vector<bool> inChild(n, false);

    for (int i = a; i <= b; ++i) {
        child[i] = p1[i];
        inChild[p1[i]] = true;
    }
    int pos = (b + 1) % n;
    int p2pos = (b + 1) % n;
    int filled = 0;
    while (filled < n - (b - a + 1)) {
        while (inChild[p2[p2pos]]) p2pos = (p2pos + 1) % n;
        child[pos] = p2[p2pos];
        inChild[p2[p2pos]] = true;
        pos = (pos + 1) % n;
        p2pos = (p2pos + 1) % n;
        ++filled;
    }
    return child;
}

Tour crossoverPMX(const Tour& p1, const Tour& p2, std::mt19937& rng) {
    int n = (int)p1.size();
    std::uniform_int_distribution<int> idxDist(0, n - 1);
    int a = idxDist(rng), b = idxDist(rng);
    if (a > b) std::swap(a, b);

    Tour child(n, -1);
    std::vector<bool> inChild(n, false);

    for (int i = a; i <= b; ++i) {
        child[i] = p1[i];
        inChild[p1[i]] = true;
    }

    std::vector<int> pos2(n);
    for (int i = 0; i < n; ++i) pos2[p2[i]] = i;

    for (int i = 0; i < n; ++i) {
        if (i >= a && i <= b) continue;
        int gene = p2[i];
        while (inChild[gene]) {
            int k = pos2[gene];
            gene = p1[k];
        }
        child[i] = gene;
        inChild[gene] = true;
    }

    return child;
}

void mutate(Tour& t, std::mt19937& rng, double prob) {
    std::uniform_real_distribution<double> probDist(0.0, 1.0);
    if (probDist(rng) > prob) return;
    int n = (int)t.size();
    std::uniform_int_distribution<int> d(0, n - 1);
    int a = d(rng), b = d(rng);
    if (a > b) std::swap(a, b);
    std::reverse(t.begin() + a, t.begin() + b + 1);
}

const Individual& tournamentSelect(const std::vector<Individual>& pop, int k,
                                   std::mt19937& rng) {
    std::uniform_int_distribution<int> d(0, (int)pop.size() - 1);
    int best = d(rng);
    for (int i = 1; i < k; ++i) {
        int idx = d(rng);
        if (pop[idx].fitness > pop[best].fitness) best = idx;
    }
    return pop[best];
}

enum class CrossoverType { OX, PMX, CX, ERX };

struct GAConfig {
    int popSize         = 50;
    int maxGenerations  = 500;
    double crossProb    = 0.85;
    double mutProb      = 0.15;
    int tournamentK     = 5;
    bool memetic        = true;
    int memeticFreq     = 5;
    int memeticIter     = 100;
    CrossoverType crossType = CrossoverType::OX;
    bool island         = true;
    int numIslands      = 4;
    int migrationInterval = 50;
    int migrationSize   = 2;
    double greedyInit   = 0.3;
};

Individual runGA(const std::vector<std::vector<double>>& dist, const GAConfig& cfg,
                 std::mt19937& rng) {
    int n = (int)dist.size();

    std::vector<Individual> pop;
    pop.reserve(cfg.popSize);
    int greedyCount = (int)(cfg.popSize * cfg.greedyInit);

    for (int i = 0; i < cfg.popSize; ++i) {
        Tour t;
        if (i < greedyCount) {
            std::uniform_int_distribution<int> startDist(0, n - 1);
            t = greedyTour(startDist(rng), dist);
        } else {
            t = randomTour(n, rng);
        }
        double d = tourDistance(t, dist);
        pop.push_back({t, 1.0 / d, d});
    }

    Individual globalBest = *std::min_element(pop.begin(), pop.end(),
        [](const Individual& a, const Individual& b){ return a.distance < b.distance; });

    auto doCrossover = [&](const Tour& p1, const Tour& p2) -> Tour {
        switch (cfg.crossType) {
            case CrossoverType::PMX: return crossoverPMX(p1, p2, rng);
            default:                 return crossoverOX(p1, p2, rng);
        }
    };

    std::uniform_real_distribution<double> probD(0.0, 1.0);

    for (int gen = 0; gen < cfg.maxGenerations; ++gen) {
        std::vector<Individual> newPop;
        newPop.reserve(cfg.popSize);

        newPop.push_back(globalBest);

        while ((int)newPop.size() < cfg.popSize) {
            const Individual& pa = tournamentSelect(pop, cfg.tournamentK, rng);
            const Individual& pb = tournamentSelect(pop, cfg.tournamentK, rng);

            Tour childTour;
            if (probD(rng) < cfg.crossProb) {
                childTour = doCrossover(pa.tour, pb.tour);
            } else {
                childTour = pa.tour;
            }

            mutate(childTour, rng, cfg.mutProb);

            if (cfg.memetic && gen % cfg.memeticFreq == 0) {
                childTour = twoOpt(childTour, dist, cfg.memeticIter);
            }

            assert(isValidTour(childTour, n) && "Crossover/mutacja produkt niepoprawnej trasy!");
            double d = tourDistance(childTour, dist);
            newPop.push_back({childTour, 1.0 / d, d});
        }

        pop = std::move(newPop);

        for (auto& ind : pop) {
            if (ind.distance < globalBest.distance) globalBest = ind;
        }
    }

    return globalBest;
}

Individual runIslandGA(const std::vector<std::vector<double>>& dist,
                       const GAConfig& cfg,
                       std::mt19937& masterRng) {
    int n = (int)dist.size();
    int numIslands = cfg.numIslands;
    int islandPopSize = cfg.popSize / numIslands;
    int totalGen = cfg.maxGenerations;
    int migInterval = cfg.migrationInterval;

    std::vector<std::vector<Individual>> islands(numIslands);
    std::vector<std::mt19937> rngs(numIslands);

    for (int isle = 0; isle < numIslands; ++isle) {
        rngs[isle] = std::mt19937(masterRng());
        auto& pop = islands[isle];
        pop.reserve(islandPopSize);
        int greedyCount = (int)(islandPopSize * cfg.greedyInit);
        for (int i = 0; i < islandPopSize; ++i) {
            Tour t;
            if (i < greedyCount) {
                std::uniform_int_distribution<int> sd(0, n - 1);
                t = greedyTour(sd(rngs[isle]), dist);
            } else {
                t = randomTour(n, rngs[isle]);
            }
            double d = tourDistance(t, dist);
            pop.push_back({t, 1.0 / d, d});
        }
    }

    std::vector<Individual> islandBests(numIslands);
    for (int isle = 0; isle < numIslands; ++isle) {
        islandBests[isle] = *std::min_element(islands[isle].begin(), islands[isle].end(),
            [](const Individual& a, const Individual& b){ return a.distance < b.distance; });
    }

    std::mutex migMtx;

    auto evolveIsland = [&](int isle, int startGen, int endGen) {
        auto& pop = islands[isle];
        auto& rng = rngs[isle];
        auto& best = islandBests[isle];
        std::uniform_real_distribution<double> probD(0.0, 1.0);

        auto doCrossover = [&](const Tour& p1, const Tour& p2) -> Tour {
            switch (cfg.crossType) {
                case CrossoverType::PMX: return crossoverPMX(p1, p2, rng);
                default:                 return crossoverOX(p1, p2, rng);
            }
        };

        for (int gen = startGen; gen < endGen; ++gen) {
            std::vector<Individual> newPop;
            newPop.reserve(islandPopSize);
            newPop.push_back(best);

            while ((int)newPop.size() < islandPopSize) {
                const Individual& pa = tournamentSelect(pop, cfg.tournamentK, rng);
                const Individual& pb = tournamentSelect(pop, cfg.tournamentK, rng);

                Tour childTour;
                if (probD(rng) < cfg.crossProb)
                    childTour = doCrossover(pa.tour, pb.tour);
                else
                    childTour = pa.tour;

                mutate(childTour, rng, cfg.mutProb);

                if (cfg.memetic && gen % cfg.memeticFreq == 0)
                    childTour = twoOpt(childTour, dist, cfg.memeticIter);

                double d = tourDistance(childTour, dist);
                newPop.push_back({childTour, 1.0 / d, d});
            }
            pop = std::move(newPop);
            for (auto& ind : pop)
                if (ind.distance < best.distance) best = ind;
        }
    };

    for (int phase = 0; phase * migInterval < totalGen; ++phase) {
        int startG = phase * migInterval;
        int endG = std::min(startG + migInterval, totalGen);

        std::vector<std::thread> threads;
        for (int isle = 0; isle < numIslands; ++isle)
            threads.emplace_back(evolveIsland, isle, startG, endG);
        for (auto& th : threads) th.join();

        std::vector<std::vector<Individual>> migrants(numIslands);
        for (int isle = 0; isle < numIslands; ++isle) {
            auto& pop = islands[isle];
            std::sort(pop.begin(), pop.end(),
                [](const Individual& a, const Individual& b){ return a.distance < b.distance; });
            for (int m = 0; m < cfg.migrationSize && m < (int)pop.size(); ++m)
                migrants[isle].push_back(pop[m]);
        }
        for (int isle = 0; isle < numIslands; ++isle) {
            int dest = (isle + 1) % numIslands;
            auto& pop = islands[dest];
            // Zastąp najgorszych
            std::sort(pop.begin(), pop.end(),
                [](const Individual& a, const Individual& b){ return a.distance > b.distance; });
            for (int m = 0; m < (int)migrants[isle].size() && m < (int)pop.size(); ++m)
                pop[m] = migrants[isle][m];
        }
    }

    Individual globalBest = islandBests[0];
    for (auto& b : islandBests)
        if (b.distance < globalBest.distance) globalBest = b;

    return globalBest;
}

struct Progress {
    std::mutex mtx;
    std::string currentName;
    std::string currentVariant;
    std::atomic<int> done{0};
    int total{0};

    void start(const std::string& name, const std::string& variant, int totalRuns) {
        std::lock_guard<std::mutex> lk(mtx);
        currentName    = name;
        currentVariant = variant;
        total          = totalRuns;
        done           = 0;
    }

    void tick() {
        int d = ++done;
        std::lock_guard<std::mutex> lk(mtx);
            int pct = total > 0 ? (d * 100) / total : 0;
            int bars = pct / 5;
            std::cout << "\r" << std::left << std::setw(10) << currentName
                      << " [" << std::setw(12) << currentVariant << "] [";
            for (int i = 0; i < 20; ++i) std::cout << (i < bars ? '#' : ' ');
            std::cout << "] " << std::setw(3) << pct << "% (" << d << "/" << total << ")   ";
        std::cout.flush();
    }

    void finish(double bestDist, double avgDist, double worstDist, double elapsed) {
        std::lock_guard<std::mutex> lk(mtx);
        std::cout << "\r" << std::left << std::setw(10) << currentName
                  << " [" << std::setw(12) << currentVariant << "] "
                  << "DONE  best=" << std::fixed << std::setprecision(2) << bestDist
                  << "  avg=" << avgDist
                  << "  worst=" << worstDist
                  << "  t=" << std::setprecision(1) << elapsed << "s        \n";
        std::cout.flush();
    }
};


void sim(const std::string& tspFile, const std::string& name,
         std::ofstream& csvFile, std::mutex& csvMtx) {

    // Wczytaj dane
    std::vector<City> cities;
    try {
        cities = loadTSP(tspFile);
    } catch (std::exception& e) {
        std::cerr << "[BLAD] " << e.what() << "\n";
        return;
    }
    auto dist = buildDistMatrix(cities);
    int n = (int)cities.size();

    GAConfig cfgBase;
    if (n <= 50)        { cfgBase.popSize = 800; cfgBase.maxGenerations = 300; cfgBase.memeticIter = 200; }
    else if (n <= 200)  { cfgBase.popSize = 500;  cfgBase.maxGenerations = 200; cfgBase.memeticIter = 100; }
    else if (n <= 1000) { cfgBase.popSize = 300;  cfgBase.maxGenerations = 150; cfgBase.memeticIter = 50;  }
    else                { cfgBase.popSize = 200;  cfgBase.maxGenerations = 80;  cfgBase.memeticIter = 20;  }

    const int RUNS = 20;

    int nThreads = 8;

    struct Variant {
        std::string label;
        CrossoverType cx;
        bool memetic;
        bool island;
    };

    std::vector<Variant> variants = {
        {"OX+Mem+Isl",  CrossoverType::OX,  true, true},
        {"PMX+Mem+Isl", CrossoverType::PMX, true, true},
    };

    Progress progress;

    for (auto& var : variants) {
        GAConfig cfg   = cfgBase;
        cfg.crossType  = var.cx;
        cfg.memetic    = var.memetic;
        cfg.island     = var.island;

        std::mutex statMtx;
        double bestDist  = 1e18;
        double worstDist = 0.0;
        double sumDist   = 0.0;
        Tour   bestTour;

        progress.start(name, var.label, RUNS);

        auto t0 = std::chrono::steady_clock::now();

        std::atomic<int> nextRun{0};

        auto worker = [&](int threadId) {
            std::mt19937 rng(std::random_device{}() ^ (uint32_t)(threadId * 2654435761u));

            while (true) {
                int runId = nextRun.fetch_add(1);
                if (runId >= RUNS) break;

                auto runStart = std::chrono::steady_clock::now();

                Individual res;
                if (cfg.island)
                    res = runIslandGA(dist, cfg, rng);
                else
                    res = runGA(dist, cfg, rng);

                double runTime = std::chrono::duration<double>(
                    std::chrono::steady_clock::now() - runStart).count();

                {
                    std::lock_guard<std::mutex> lk(csvMtx);

                    std::ostringstream route;
                    for (int i = 0; i < (int)res.tour.size(); ++i) {
                        if (i) route << "|";
                        const City& c = cities[res.tour[i]];
                        route << c.id << ":" << std::fixed << std::setprecision(4) << c.x << ":" << c.y;
                    }
                    if (!res.tour.empty()) {
                        const City& c0 = cities[res.tour[0]];
                        route << "|" << c0.id << ":" << std::fixed << std::setprecision(4) << c0.x << ":" << c0.y;
                    }

                    csvFile << name                                               // country
                            << "," << n                                           // cities
                            << "," << runId                                       // simulation_id
                            << "," << var.label                                   // crossover
                            << "," << std::fixed << std::setprecision(2) << res.distance  // cost
                            << "," << cfgBase.maxGenerations                     // steps
                            << ",\"" << route.str() << "\""                   // route_points (quoted)
                            << "\n";

                    csvFile.flush();
                }

                {
                    std::lock_guard<std::mutex> lk(statMtx);
                    sumDist += res.distance;
                    if (res.distance < bestDist) { bestDist = res.distance; bestTour = res.tour; }
                    if (res.distance > worstDist)   worstDist = res.distance;
                }

                progress.tick();
            }
        };

        std::vector<std::thread> threads;
        threads.reserve(nThreads);
        for (int t = 0; t < nThreads; ++t)
            threads.emplace_back(worker, t);
        for (auto& th : threads) th.join();

        double elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - t0).count();

        double avgDist = sumDist / RUNS;

        progress.finish(bestDist, avgDist, worstDist, elapsed);
    }
}

int main() {
    const std::string csvPath = "wyniki_ga2.csv";
    std::ofstream csvFile(csvPath);
    if (!csvFile.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku CSV: " << csvPath << "\n";
        return 1;
    }

    csvFile << "country,cities,simulation_id,crossover,cost,steps,route_points\n";

    std::mutex csvMtx;

    struct Dataset { std::string file; std::string name; };
    std::vector<Dataset> datasets = {
                {"uy734.tsp",  "Uruguay"},
{"ca4663.tsp", "Canada"},
        {"mu1979.tsp", "Oman"},
        {"tz6117.tsp", "Tanzania"},
        {"eg7146.tsp", "Egypt"},
        {"ei8246.tsp", "Ireland"},
        {"wi29.tsp",   "Sahara"},
        {"qa194.tsp",  "Qatar"},
        {"dj38.tsp",   "Djibouti"},
        {"zi929.tsp",  "Zimbabwe"},
    };

    int nThreads = 8;

    std::cout << "Liczba watkow: " << nThreads << "\n";
    std::cout << "Datasety przetwarzane sekwencyjnie, runy rownolegle.\n\n";

    for (auto& ds : datasets) {
        std::cout << "=== " << ds.name << " (" << ds.file << ") ===\n";
        sim(ds.file, ds.name, csvFile, csvMtx);
        std::cout << "\n";
    }

    csvFile.close();
    std::cout << "Zakonczone! Wyniki zapisane do: " << csvPath << "\n";
    return 0;
}