#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

const int a = 2, b = 8, c = 3, d = 9, e = 8, f = 2;

bool is_less_or_equal(const vector<int>& v1, const vector<int>& v2) {
    if (v1.size() != v2.size()) return false;
    for (size_t i = 0; i < v1.size(); ++i) {
        if (v1[i] > v2[i]) return false;
    }
    return true;
}

vector<vector<int>> find_minimal_elements(const vector<vector<int>>& S) {
    vector<vector<int>> minimal_elements;
    
    for (size_t i = 0; i < S.size(); ++i) {
        bool is_minimal = true;
        for (size_t j = 0; j < S.size(); ++j) {
            if (i == j) continue;

            if (is_less_or_equal(S[j], S[i]) && S[j] != S[i]) {
                is_minimal = false;
                break;
            }
        }

        if (is_minimal) {
            if (find(minimal_elements.begin(), minimal_elements.end(), S[i]) == minimal_elements.end()) {
                minimal_elements.push_back(S[i]);
            }
        }
    }
    return minimal_elements;
}

void print_vector(const vector<int>& v) {
    cout << "(";
    for (size_t i = 0; i < v.size(); ++i) {
        cout << v[i] << (i < v.size() - 1 ? ", " : "");
    }
    cout << ")";
}

void print_relation(const vector<int>& v1, const vector<int>& v2) {
    print_vector(v1);
    if (is_less_or_equal(v1, v2)) cout << " <= ";
    else if (is_less_or_equal(v2, v1)) cout << " >= ";
    else cout << " nieporownywalne z ";
    print_vector(v2);
    cout << "\n";
}

int main() {
    cout << "Zmienne: a="<<a<<", b="<<b<<", c="<<c<<", d="<<d<<", e="<<e<<", f="<<f<<"\n\n";
    
    cout << "a)\n";
    vector<int> p1 = {a, b};
    vector<int> p2 = {c, d};
    vector<int> p3 = {e, f};
    
    print_relation(p1, p2);
    print_relation(p1, p3);
    print_relation(p2, p3);
    
    cout << "\nb)\n";
    vector<int> t1 = {a, c, e};
    vector<int> t2 = {b, d, f};
    print_relation(t1, t2);

    cout << "\nA:\n";
    vector<vector<int>> setA;
    int max_radius_A = ceil(sqrt(5));
    for (int x = max(0, a - max_radius_A); x <= a + max_radius_A; ++x) {
        for (int y = max(0, b - max_radius_A); y <= b + max_radius_A; ++y) {
            if (pow(x - a, 2) + pow(y - b, 2) < 5) {
                setA.push_back({x, y});
            }
        }
    }
    
    vector<vector<int>> minA = find_minimal_elements(setA);
    for (const auto& v : minA) {
        print_vector(v); cout << " ";
    }
    cout << "\n";

    cout << "\nB:\n";
    vector<vector<int>> setB;
    int search_limit = max({c, d, e, f}) + 16;
    
    for (int x1 = 0; x1 <= search_limit; ++x1) {
        for (int x2 = 0; x2 <= search_limit; ++x2) {
            for (int x3 = 0; x3 <= search_limit; ++x3) {
                for (int x4 = 0; x4 <= search_limit; ++x4) {
                    long long val = pow(x1 - c, 2) + pow(x2 - d, 2) + pow(x3 - e, 2) + pow(x4 - f, 2);
                    if (val > 224) {
                        setB.push_back({x1, x2, x3, x4});
                    }
                }
            }
        }
    }
    
    vector<vector<int>> minB = find_minimal_elements(setB);
    for (const auto& v : minB) {
        print_vector(v); cout << " ";
    }
    cout << "\n";

    return 0;
}