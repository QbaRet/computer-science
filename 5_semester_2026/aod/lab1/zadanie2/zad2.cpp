#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <chrono>

class TopologicalSort {
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> in_degree;

public:
    TopologicalSort(int n) : n(n), adj(n + 1), in_degree(n + 1, 0) {}

    void add_edge(int u, int v) {
        adj[u].push_back(v);
        in_degree[v]++;
    }

    void run() const {
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<int> deg = in_degree;
        std::queue<int> q;

        for (int i = 1; i <= n; ++i) {
            if (deg[i] == 0) q.push(i);
        }

        std::vector<int> topo;
        topo.reserve(n);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            topo.push_back(u);

            for (int v : adj[u]) {
                if (--deg[v] == 0) {
                    q.push(v);
                }
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        if (static_cast<int>(topo.size()) == n) {
            std::cout << "Graf jest acykliczny.\n";
            if (n <= 200) {
                std::cout << "Porzadek topologiczny: ";
                for (int v : topo) std::cout << v << " ";
                std::cout << "\n";
            }
        } else {
            std::cout << "Graf zawiera cykl.\n";
        }
        std::cout << "Czas: " << elapsed.count() << " ms\n";
    }
};

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::ifstream file;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            file.open(argv[i]);
            if (file.is_open()) {
                std::cin.rdbuf(file.rdbuf());
            }
        }
    }

    char type;
    if (!(std::cin >> type)) return 0;

    int n, m;
    std::cin >> n >> m;

    TopologicalSort g(n);
    for (int i = 0; i < m; ++i) {
        int u, v;
        std::cin >> u >> v;
        g.add_edge(u, v);
    }

    g.run();
    return 0;
}
