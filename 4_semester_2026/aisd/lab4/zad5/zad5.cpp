#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <queue>
 
using namespace std;
using namespace chrono;

const int N_MIN = 10;
const int N_MAX = 1010;
const int STEP  = 20;
const int REP   = 20;
 
const int MAXN = N_MAX + 1;
 
double macierz[MAXN][MAXN];         
int    rodzic[MAXN];                 
 
mt19937 rng(42);
uniform_real_distribution<double> losuj(0.0, 1.0);
 

 
struct Krawedz {
    double w;
    int u, v;
};
 
vector<pair<double, int>> adjList[MAXN];
 
void generujGraf(int n) {
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            double w = losuj(rng);
            macierz[i][j] = w;
            macierz[j][i] = w;
        }
 
    for (int i = 0; i < n; i++) {
        adjList[i].clear();
        for (int j = 0; j < n; j++)
            if (i != j)
                adjList[i].push_back({macierz[i][j], j});
    }
}
 

double primMacierz(int n) {
    vector<double> minWaga(n, 1e18);
    vector<bool>   wMST(n, false);
    minWaga[0] = 0.0;
    double suma = 0.0;
 
    for (int iter = 0; iter < n; iter++) {
        int u = -1;
        for (int v = 0; v < n; v++)
            if (!wMST[v] && (u == -1 || minWaga[v] < minWaga[u]))
                u = v;
 
        wMST[u] = true;
        suma += minWaga[u];
 
        for (int v = 0; v < n; v++)
            if (!wMST[v] && macierz[u][v] < minWaga[v])
                minWaga[v] = macierz[u][v];
    }
    return suma;
}

double primKopiec(int n) {
    vector<double> minWaga(n, 1e18);
    vector<bool>   wMST(n, false);
    minWaga[0] = 0.0;
    double suma = 0.0;
 
    priority_queue<pair<double,int>,
                   vector<pair<double,int>>,
                   greater<pair<double,int>>> pq;
    pq.push({0.0, 0});
 
    while (!pq.empty()) {
        auto [w, u] = pq.top();
        pq.pop();
 
        if (wMST[u]) continue;   
        wMST[u] = true;
        suma += w;
 
        for (auto [wagaKraw, v] : adjList[u]) {
            if (!wMST[v] && wagaKraw < minWaga[v]) {
                minWaga[v] = wagaKraw;
                pq.push({wagaKraw, v});
            }
        }
    }
    return suma;
}
 

int znajdz(int x) {
    if (rodzic[x] != x)
        rodzic[x] = znajdz(rodzic[x]);
    return rodzic[x];
}
 
bool polacz(int a, int b) {
    a = znajdz(a); b = znajdz(b);
    if (a == b) return false;
    rodzic[b] = a;
    return true;
}
 
bool porownajKrawedzie(const Krawedz& a, const Krawedz& b) {
    return a.w < b.w;
}
 
double kruskal(int n) {
    vector<Krawedz> krawedzie;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            krawedzie.push_back({macierz[i][j], i, j});
 
    sort(krawedzie.begin(), krawedzie.end(), porownajKrawedzie);
 
    for (int i = 0; i < n; i++)
        rodzic[i] = i;
 
    double suma = 0.0;
    int dodano = 0;
    for (int k = 0; k < (int)krawedzie.size(); k++) {
        if (polacz(krawedzie[k].u, krawedzie[k].v)) {
            suma += krawedzie[k].w;
            if (++dodano == n - 1) break;
        }
    }
    return suma;
}
 

int main() {
    ofstream plik("wyniki.csv");
    plik << "n,prim_macierz_us,prim_kopiec_us,kruskal_us\n";
 
    cout << setw(6)  << "n"
         << setw(20) << "Prim macierz [us]"
         << setw(20) << "Prim kopiec [us]"
         << setw(18) << "Kruskal [us]" << "\n";
    cout << string(64, '-') << "\n";
 
    for (int n = N_MIN; n <= N_MAX; n += STEP) {
        long long sumaMacierz = 0, sumaKopiec = 0, sumaKruskal = 0;
 
        for (int r = 0; r < REP; r++) {
            generujGraf(n);
 
            auto t0 = high_resolution_clock::now();
            primMacierz(n);
            auto t1 = high_resolution_clock::now();
            primKopiec(n);
            auto t2 = high_resolution_clock::now();
            kruskal(n);
            auto t3 = high_resolution_clock::now();
 
            sumaMacierz += duration_cast<microseconds>(t1 - t0).count();
            sumaKopiec  += duration_cast<microseconds>(t2 - t1).count();
            sumaKruskal += duration_cast<microseconds>(t3 - t2).count();
        }
 
        double srMacierz = (double)sumaMacierz / REP;
        double srKopiec  = (double)sumaKopiec  / REP;
        double srKruskal = (double)sumaKruskal / REP;
 
        cout << fixed << setprecision(1)
             << setw(6)  << n
             << setw(20) << srMacierz
             << setw(20) << srKopiec
             << setw(18) << srKruskal << "\n";
 
        plik << n << "," << srMacierz << "," << srKopiec << "," << srKruskal << "\n";
    }
 
    plik.close();
    return 0;
}