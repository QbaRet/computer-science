#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;

long long porownania = 0;
long long przestawienia = 0;
long long licznik_krokow = 0;

bool compare(int a, int b) {
    porownania++;
    return a > b; 
}

void set_element(int &dest, int value) {
    przestawienia++;
    dest = value;
}

void printArray(const vector<int>& arr) {
    for (int x : arr) {
        cout << setw(2) << setfill('0') << x << " ";
    }
    cout << endl;
}

bool isSorted(const vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < arr[i - 1]) return false;
    }
    return true;
}

struct Run {
    int start, end;
    int length() const { return end - start + 1; }
};

void insertionSort(vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        przestawienia++; 
        int j = i - 1;
        
        while (j >= left && compare(arr[j], key)) {
            set_element(arr[j + 1], arr[j]);
            j--;
        }
        set_element(arr[j + 1], key);
    }
}

void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> tmp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right) {
        if (!compare(arr[i], arr[j])) {
            tmp[k++] = arr[i++];
        } else {
            tmp[k++] = arr[j++];
        }
    }

    while (i <= mid) tmp[k++] = arr[i++];
    while (j <= right) tmp[k++] = arr[j++];

    for (int x = 0; x < k; x++) {
        set_element(arr[left + x], tmp[x]);
    }
}

const int MIN_RUN = 4;

vector<Run> detectRuns(vector<int>& arr, int n) {
    vector<Run> runs;
    int i = 0;
    while (i < n) {
        int j = i + 1;
        
        while (j < n && !compare(arr[j - 1], arr[j])) {
            j++;
        }

        int runEnd = min(i + MIN_RUN - 1, n - 1);
        if (j - 1 < runEnd) {
            insertionSort(arr, i, runEnd);
            j = runEnd + 1;
        }

        runs.push_back({i, j - 1});
        i = j;
    }
    return runs;
}

void shortestPairSort(vector<int>& arr, int n) {
    if (n <= 1) return;
    
    vector<Run> runs = detectRuns(arr, n);
    
    while (runs.size() > 1) {
        int best = 0;
        int bestLen = runs[0].length() + runs[1].length();

        for (int i = 1; i + 1 < (int)runs.size(); i++) {
            int len = runs[i].length() + runs[i + 1].length();
            if (len < bestLen) {
                bestLen = len;
                best = i;
            }
        }

        merge(arr, runs[best].start, runs[best].end, runs[best + 1].end);

        if (arr.size() < 40) {
            cout << "Krok " << ++licznik_krokow << ": ";
            printArray(arr);
        }

        runs[best].end = runs[best + 1].end;
        runs.erase(runs.begin() + best + 1);
    }
}

int main(int argc, char* argv[]) {
    int n;
    if (!(cin >> n)) return 0;

    vector<int> arr(n);
    vector<int> original(n);

    for (int i = 0; i < n; i++) {
        cin >> arr[i];
        original[i] = arr[i];
    }

    if (n < 40) {
        cout << "Tablica wejsciowa:  ";
        printArray(original);
    }

    shortestPairSort(arr, n);

    if (argc > 1) {
        ofstream plik(argv[1], ios::app);
        if (plik.is_open()) {
            plik << "ShortestPairSort;" << n << ";" << porownania << ";" << przestawienia << endl;
            plik.close();
        }
    }

    if (n < 40) {
        cout << "Tablica wejsciowa:  ";
        printArray(original);
        cout << "Tablica wynikowa:   ";
        printArray(arr);
    }

    cout << "\nSTATYSTYKI:" << endl;
    cout << "Liczba n:           " << n << endl;
    cout << "Liczba porownan:    " << porownania << endl;
    cout << "Liczba przestawien: " << przestawienia << endl;

    return 0;
}