#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <chrono>

class BipartiteChecker {
    int n;
    std::vector<std::vector<int>> adj;

public:
    BipartiteChecker(int n) : n(n), adj(n + 1) {}

    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void check_bipartite() const {
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<int> color(n + 1, -1);
        bool is_bipartite = true;

        for (int i = 1; i <= n; ++i) {
            if (color[i] != -1) continue;

            std::queue<int> q;
            color[i] = 0;
            q.push(i);

            while (!q.empty()) {
                int u = q.front();
                q.pop();

                for (int v : adj[u]) {
                    if (color[v] == -1) {
                        color[v] = 1 - color[u];
                        q.push(v);
                    } else if (color[v] == color[u]) {
                        is_bipartite = false;
                        break;
                    }
                }
                if (!is_bipartite) break;
            }
            if (!is_bipartite) break;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        if (is_bipartite) {
            std::cout << "Graf jest dwudzielny.\n";
            if (n <= 200) {
                std::cout << "V1: ";
                for (int i = 1; i <= n; ++i) {
                    if (color[i] == 0) std::cout << i << " ";
                }
                std::cout << "\nV2: ";
                for (int i = 1; i <= n; ++i) {
                    if (color[i] == 1) std::cout << i << " ";
                }
                std::cout << "\n";
            }
        } else {
            std::cout << "Graf nie jest dwudzielny.\n";
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

    BipartiteChecker graph(n);
    for (int i = 0; i < m; ++i) {
        int u, v;
        std::cin >> u >> v;
        graph.add_edge(u, v);
    }

    graph.check_bipartite();
    return 0;
}
