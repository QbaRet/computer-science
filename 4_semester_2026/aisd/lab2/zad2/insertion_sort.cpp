#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream> 

using namespace std;

long long porownania = 0;
long long przestawienia = 0;

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

void insertionSort(vector<int>& arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        przestawienia++;
        int j = i - 1;

        while (j >= 0 && compare(arr[j], key)) {
            set_element(arr[j + 1], arr[j]);
            j--;
        }
        set_element(arr[j + 1], key);

        if (n < 40) {
            cout << "Krok " << i << ": ";
            printArray(arr);
        }
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

    insertionSort(arr, n);

    
    if (argc > 1) { 
        ofstream plik(argv[1], ios::app); 
        if (plik.is_open()) {
            
            plik << "InsertionSort;" << n << ";" << porownania << ";" << przestawienia << endl;
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