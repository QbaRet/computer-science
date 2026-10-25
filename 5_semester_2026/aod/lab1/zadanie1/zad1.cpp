#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <chrono>

struct Edge {
    int u, v;
};

class Graph {
    int n;
    bool directed;
    std::vector<std::vector<int>> adj;

public:
    Graph(int n, bool directed) : n(n), directed(directed), adj(n + 1) {}

    void add_edge(int u, int v) {
        adj[u].push_back(v);
        if (!directed) {
            adj[v].push_back(u);
        }
    }

    void bfs(bool print_tree) const {
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<bool> visited(n + 1, false);
        std::vector<int> order;
        std::vector<Edge> tree;

        for (int i = 1; i <= n; ++i) {
            if (!visited[i]) {
                std::queue<int> q;
                visited[i] = true;
                q.push(i);

                while (!q.empty()) {
                    int u = q.front();
                    q.pop();
                    order.push_back(u);

                    for (int v : adj[u]) {
                        if (!visited[v]) {
                            visited[v] = true;
                            tree.push_back({u, v});
                            q.push(v);
                        }
                    }
                }
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        std::cout << "BFS:\n";
        if (n <= 200) {
            std::cout << "Kolejnosc: ";
            for (int v : order) std::cout << v << " ";
            std::cout << "\n";

            if (print_tree) {
                std::cout << "Drzewo:\n";
                for (const auto& e : tree) {
                    std::cout << e.u << " -> " << e.v << "\n";
                }
            }
        }
        std::cout << "Czas: " << elapsed.count() << " ms\n\n";
    }

    void dfs(bool print_tree) const {
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<bool> visited(n + 1, false);
        std::vector<int> order;
        std::vector<Edge> tree;
        std::vector<std::pair<int, size_t>> st;

        for (int i = 1; i <= n; ++i) {
            if (!visited[i]) {
                visited[i] = true;
                order.push_back(i);
                st.push_back({i, 0});

                while (!st.empty()) {
                    int u = st.back().first;
                    size_t& idx = st.back().second;

                    if (idx < adj[u].size()) {
                        int v = adj[u][idx++];
                        if (!visited[v]) {
                            visited[v] = true;
                            tree.push_back({u, v});
                            order.push_back(v);
                            st.push_back({v, 0});
                        }
                    } else {
                        st.pop_back();
                    }
                }
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        std::cout << "DFS:\n";
        if (n <= 200) {
            std::cout << "Kolejnosc: ";
            for (int v : order) std::cout << v << " ";
            std::cout << "\n";

            if (print_tree) {
                std::cout << "Drzewo:\n";
                for (const auto& e : tree) {
                    std::cout << e.u << " -> " << e.v << "\n";
                }
            }
        }
        std::cout << "Czas: " << elapsed.count() << " ms\n";
    }
};

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool print_tree = false;
    std::ifstream file;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--tree" || arg == "-t") {
            print_tree = true;
        } else if (arg[0] != '-') {
            file.open(arg);
            if (file.is_open()) {
                std::cin.rdbuf(file.rdbuf());
            }
        }
    }

    char type;
    if (!(std::cin >> type)) return 0;

    int n, m;
    std::cin >> n >> m;

    Graph g(n, type == 'D' || type == 'd');
    for (int i = 0; i < m; ++i) {
        int u, v;
        std::cin >> u >> v;
        g.add_edge(u, v);
    }

    g.bfs(print_tree);
    g.dfs(print_tree);

    return 0;
}
