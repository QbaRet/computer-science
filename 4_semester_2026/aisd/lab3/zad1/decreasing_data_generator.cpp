#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) return 1; 
    int n = stoi(argv[1]);
    int max_range = 2 * n - 1;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, max_range);
    cout << n << endl;
    vector<int> arr(n);
    for(int i = 0; i < n; i++) {
        arr[i] = dis(gen);
    }
    sort(arr.begin(), arr.end());
    reverse(arr.begin(), arr.end());
    for(int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    return 0;
}