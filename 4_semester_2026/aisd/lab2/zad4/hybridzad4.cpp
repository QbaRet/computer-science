#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iomanip>

using namespace std;

bool staircaseSearch(const vector<vector<int>>& matrix, int target, long long &comps) {
    int m = matrix.size();
    int n = matrix[0].size();
    int i = 0;
    int j = n - 1;
    while (i < m && j >= 0) {
        comps++;
        if (matrix[i][j] == target) return true;
        comps++; 
        if (matrix[i][j] > target) j--;
        else i++;
    }
    return false;
}
bool binarySearchInRows(const vector<vector<int>>& matrix, int target, long long &comps) {
    int m = matrix.size();
    for (int i = 0; i < m; i++) {
        int low = 0, high = matrix[i].size() - 1;
        while (low <= high) {
            comps++;
            int mid = low + (high - low) / 2;
            if (matrix[i][mid] == target) return true;
            comps++;
            if (matrix[i][mid] < target) low = mid + 1;
            else high = mid - 1;
        }
    }
    return false;
}

vector<vector<int>> generateMatrix(int m, int n) {
    vector<vector<int>> matrix(m, vector<int>(n));
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = (i + j) * 2;
        }
    }
    return matrix;
}

int main() {
    ofstream file("wyniki_macierz.csv");
    file << "m;n;ratio;staircase_comps;binary_comps\n";

    cout << "Porownanie wyszukiwania w macierzy\n"; 
    
    long long TOTAL_ELEMENTS = 1000000; 

    for (int m = 1; m <= 1000; m += (m < 100 ? 10 : 100)) {
        int n = TOTAL_ELEMENTS / m;
        auto matrix = generateMatrix(m, n);
        int target = -1; 
        long long compsStair = 0;
        long long compsBin = 0;

        bool foundStair = staircaseSearch(matrix, target, compsStair);
        bool foundBin = binarySearchInRows(matrix, target, compsBin);

        double ratio = (double)n / m;
        file << m << ";" << n << ";" << ratio << ";" << compsStair << ";" << compsBin << endl;
                cout << "Przetestowano: " << setw(5) << m << "x" << setw(7) << n
                         << " | Binary: " << setw(8) << compsBin
                         << "" << (foundBin ? "" : "") << ""
                         << " | Staircase: " << setw(8) << compsStair
                         << "" << (foundStair ? "" : "") << "\n";
    }

    file.close();
    return 0;
}