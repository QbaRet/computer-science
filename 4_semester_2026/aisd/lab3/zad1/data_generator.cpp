#include <iostream>
#include <vector>
#include <random>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 1; 
    }
    
    int n = stoi(argv[1]);
    int max_range = 2 * n - 1;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, max_range);
    
    int k;
    if (argc >= 3) {
        k = stoi(argv[2]);
        if (k < 1 || k > n) {
            cerr << "Blad generatora: k musi byc w przedziale [1, " << n << "]." << endl;
            return 1;
        }
    } else {
        uniform_int_distribution<> dis_k(1, n);
        k = dis_k(gen);
    }

    cout << n << " " << k << endl;

    for(int i = 0; i < n; i++) {
        cout << dis(gen) << " ";
    }
    cout << endl;

    return 0;
}