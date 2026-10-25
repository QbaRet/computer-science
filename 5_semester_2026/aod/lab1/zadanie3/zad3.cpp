#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

class SCCKosaraju {
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<std::vector<int>> adj_rev;

public:
    SCCKosaraju(int n) : n(n), adj(n + 1), adj_rev(n + 1) {}

    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj_rev[v].push_back(u);
    }

    void dfs1(int start_node, std::vector<bool>& visited, std::vector<int>& order) const {
        std::vector<std::pair<int, size_t>> st;
        visited[start_node] = true;
        st.push_back({start_node, 0});

        while (!st.empty()) {
            int u = st.back().first;
            size_t& idx = st.back().second;

            if (idx < adj[u].size()) {
                int v = adj[u][idx++];
                if (!visited[v]) {
                    visited[v] = true;
                    st.push_back({v, 0});
                }
            } else {
                order.push_back(u);
                st.pop_back();
            }
        }
    }

    void dfs2(int start_node, std::vector<bool>& visited, std::vector<int>& component) const {
        std::vector<int> st;
        visited[start_node] = true;
        st.push_back(start_node);

        while (!st.empty()) {
            int u = st.back();
            st.pop_back();
            component.push_back(u);

            for (int v : adj_rev[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    st.push_back(v);
                }
            }
        }
    }

    void find_scc() const {
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<bool> visited(n + 1, false);
        std::vector<int> order;
        order.reserve(n);

        for (int i = 1; i <= n; ++i) {
            if (!visited[i]) {
                dfs1(i, visited, order);
            }
        }

        std::fill(visited.begin(), visited.end(), false);
        std::vector<std::vector<int>> scc;

        for (int i = n - 1; i >= 0; --i) {
            int u = order[i];
            if (!visited[u]) {
                std::vector<int> component;
                dfs2(u, visited, component);
                scc.push_back(component);
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        std::cout << "Liczba silnie spojnych skladowych: " << scc.size() << "\n";
        for (size_t i = 0; i < scc.size(); ++i) {
            std::cout << "Skladowa " << (i + 1) << " (rozmiar " << scc[i].size() << ")";
            if (n <= 200) {
                std::cout << ": ";
                for (int v : scc[i]) std::cout << v << " ";
            }
            std::cout << "\n";
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

    SCCKosaraju g(n);
    for (int i = 0; i < m; ++i) {
        int u, v;
        std::cin >> u >> v;
        g.add_edge(u, v);
    }

    g.find_scc();
    return 0;
}
