#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream> 

using namespace std;

long long porownania = 0;
long long przestawienia = 0;

bool compare_lt(int a, int b) {
    porownania++;
    return a < b;
}

bool compare_gt(int a, int b) { 
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

void hybridQSIS(vector<int>& arr, int left, int right) {
    int threshold = 10;
    
    if (right - left + 1 <= threshold) {
        for (int i = left + 1; i <= right; i++) {
            int key = arr[i];
            przestawienia++; 
            int j = i - 1;
            while (j >= left && compare_gt(arr[j], key)) { 
                set_element(arr[j + 1], arr[j]);
                j--;
            }
            set_element(arr[j + 1], key);
        }
    } else {
        int i = left, j = right, pivot = arr[(left + right) / 2];
        do {
            while (compare_lt(arr[i], pivot)) i++; 
            while (compare_gt(arr[j], pivot)) j--; 
            if (i <= j) {
                int temp = arr[i];
                set_element(arr[i], arr[j]);
                set_element(arr[j], temp);
                
                if (arr.size() < 40) {
                    cout << "QS Part (pivot " << pivot << "): ";
                    printArray(arr);
                }
                i++;
                j--;
            }
        } while (i <= j);
        
        if (left < j) hybridQSIS(arr, left, j);
        if (i < right) hybridQSIS(arr, i, right);
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

    if (n > 0) {
        hybridQSIS(arr, 0, n - 1);
    }

    
    if (argc > 1) {
        ofstream plik(argv[1], ios::app);
        if (plik.is_open()) {
            plik << "HybridSort;" << n << ";" << porownania << ";" << przestawienia << endl;
            plik.close();
        }
    }

    if (n < 40) {
        cout << "Tablica wynikowa:   ";
        printArray(arr);
    }

    cout << "\nSTATYSTYKI:" << endl;
    cout << "Liczba n:           " << n << endl;
    cout << "Liczba porownan:    " << porownania << endl;
    cout << "Liczba przestawien: " << przestawienia << endl;
    cout << "Czy posortowane:    " << (isSorted(arr) ? "TAK" : "NIE") << endl;

    return 0;
}