#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <climits>

using namespace std;
using namespace chrono;

const int N_MIN = 10;
const int N_MAX = 1010;
const int STEP  = 20;
const int REP   = 20; 

vector<vector<int>> generuj_mst_prim(int n, mt19937& rng) {
    vector<vector<double>> macierz(n, vector<double>(n, 0.0));
    uniform_real_distribution<double> losuj(0.0, 1.0);
    
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double w = losuj(rng);
            macierz[i][j] = w;
            macierz[j][i] = w;
        }
    }

    vector<double> minWaga(n, 1e18);
    vector<bool> wMST(n, false);
    vector<int> rodzic(n, -1); 
    minWaga[0] = 0.0;

    for (int iter = 0; iter < n; iter++) {
        int u = -1;
        for (int v = 0; v < n; v++) {
            if (!wMST[v] && (u == -1 || minWaga[v] < minWaga[u])) {
                u = v;
            }
        }

        wMST[u] = true;

        for (int v = 0; v < n; v++) {
            if (!wMST[v] && macierz[u][v] < minWaga[v]) {
                minWaga[v] = macierz[u][v];
                rodzic[v] = u; 
            }
        }
    }

    vector<vector<int>> mst(n);
    for (int i = 1; i < n; i++) {
        if (rodzic[i] != -1) {
            mst[i].push_back(rodzic[i]);
            mst[rodzic[i]].push_back(i);
        }
    }
    
    return mst;
}



void ukorzen_drzewo(const vector<vector<int>>& adj, int root, vector<int>& parent, vector<vector<int>>& children) {
    int n = adj.size();
    parent.assign(n, -1);
    children.assign(n, {});
    
    vector<int> stack; 
    stack.reserve(n);
    stack.push_back(root);
    parent[root] = root; 
    while (!stack.empty()) {
        int u = stack.back(); 
        stack.pop_back();
        for (int v : adj[u]) {
            if (v != parent[u]) {
                parent[v] = u;
                children[u].push_back(v);
                stack.push_back(v);
            }
        }
    }
}

int dfs_rundy(int u, const vector<vector<int>>& children, vector<int>& T) {
    if (children[u].empty()) {
        T[u] = 0;
        return 0; 
    }
    
    int m = children[u].size();
    vector<int> czasy_dzieci; 
    czasy_dzieci.reserve(m);
    
    for (int v : children[u]) {
        czasy_dzieci.push_back(dfs_rundy(v, children, T));
    }
    
    sort(czasy_dzieci.begin(), czasy_dzieci.end(), greater<int>());
    
    int best = 0;
    for (int i = 0; i < m; ++i) {
        int finish = (i + 1) + czasy_dzieci[i]; 
        if (finish > best) best = finish;
    }
    
    T[u] = best;
    return best;
}

int oblicz_rundy(const vector<vector<int>>& mst, int root) {
    int n = mst.size();
    vector<int> parent;
    vector<vector<int>> children;
    
    ukorzen_drzewo(mst, root, parent, children);
    vector<int> T(n, 0);
    
    return dfs_rundy(root, children, T);
}



int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ofstream plik("wyniki_zad6.csv");
    plik << "n,srednia_rund,min_rund,max_rund\n";

    cout << setw(6)  << "n"
         << setw(15) << "Srednia"
         << setw(10) << "Min"
         << setw(10) << "Max" << "\n";
    cout << string(41, '-') << "\n";

    mt19937 rng(42); 
    for (int n = N_MIN; n <= N_MAX; n += STEP) {
        long long suma_rund = 0;
        int min_rund = INT_MAX;
        int max_rund = 0;

        for (int r = 0; r < REP; r++) {
            auto mst = generuj_mst_prim(n, rng);
            
            uniform_int_distribution<int> losuj_korzen(0, n - 1);
            int start_root = losuj_korzen(rng);
            
            int rundy = oblicz_rundy(mst, start_root);
            
            suma_rund += rundy;
            if (rundy < min_rund) min_rund = rundy;
            if (rundy > max_rund) max_rund = rundy;
        }

        double srednia_rund = (double)suma_rund / REP;

        cout << fixed << setprecision(2)
             << setw(6)  << n
             << setw(15) << srednia_rund
             << setw(10) << min_rund
             << setw(10) << max_rund << "\n";

        plik << n << "," << srednia_rund << "," << min_rund << "," << max_rund << "\n";
    }

    plik.close();

    return 0;
}