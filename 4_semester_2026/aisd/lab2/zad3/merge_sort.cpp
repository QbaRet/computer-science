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





void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    
    vector<int> L(n1), R(n2);

    
    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
        przestawienia++;
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
        przestawienia++;
    }

    int i = 0, j = 0, k = left;

    
    while (i < n1 && j < n2) {
        
        if (!compare(L[i], R[j])) {
            set_element(arr[k], L[i]);
            i++;
        } else {
            set_element(arr[k], R[j]);
            j++;
        }
        k++;
    }

    
    while (i < n1) {
        set_element(arr[k], L[i]);
        i++;
        k++;
    }

    
    while (j < n2) {
        set_element(arr[k], R[j]);
        j++;
        k++;
    }
}

void mergeSort(vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);

        
        if (arr.size() < 40) {
            cout << "Scalanie [" << left << "-" << mid << "] i [" << mid+1 << "-" << right << "]: ";
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

    
    mergeSort(arr, 0, n - 1);

    
    if (argc > 1) {
        ofstream plik(argv[1], ios::app);
        if (plik.is_open()) {
            
            plik << "MergeSort;" << n << ";" << porownania << ";" << przestawienia << endl;
            plik.close();
        }
    }

    if (n < 40) {
        cout << "Tablica wynikowa:   ";
        printArray(arr);
    }
    return 0;
}