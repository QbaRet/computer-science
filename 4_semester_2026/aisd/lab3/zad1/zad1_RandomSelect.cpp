#include<iostream>
#include<vector>
#include<cstdlib> 
#include<ctime>  
#include<iomanip>
#include<algorithm>

using namespace std;

long long porownania = 0;
long long przestawienia = 0;
long long licznik_partycji = 0;
int n_global = 0;

bool compare(int a, int b) {
    porownania++;
    return a <= b;
}

void custom_swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
    przestawienia += 2; 
}

void printArray(const vector<int>& arr) {
    for (int x : arr) {
        cout << setw(2) << setfill('0') << x << " ";
    }
    cout << endl;
}

int randomPartition(vector<int>& arr, int left, int right){
    int random_index = left + rand() % (right - left + 1);
    custom_swap(arr[random_index], arr[right]);

    int pivot = arr[right];
    int i = left;
    
    for(int j = left; j < right; j++){
        if(compare(arr[j], pivot)){
            custom_swap(arr[i], arr[j]);
            i++; 
        }
    }
    custom_swap(arr[i], arr[right]);

    if (n_global <= 30) {
        cout << "Po partycji " << ++licznik_partycji << " (pivot: " << setw(2) << setfill('0') << pivot << "): ";
        printArray(arr);
    }

    return i;
}

int randomSelect(vector<int>& arr, int left, int right, int i){
    if(left >= right){
        return arr[left];
    }
    int q = randomPartition(arr, left, right);
    int k = q - left + 1;

    if(i == k){
        return arr[q]; 
    }
    else if(i < k){
        return randomSelect(arr, left, q - 1, i); 
    }
    else{
        return randomSelect(arr, q + 1, right, i - k);
    }
}

int main(){
    srand(time(NULL));

    int n, k;
    
    if (!(cin >> n)) return 0;
    if (!(cin >> k)) return 0;
    
    if (k < 1 || k > n) {
        cerr << "Blad: Szukana statystyka k (" << k << ") musi byc w przedziale [1, " << n << "]." << endl;
        return 1;
    }

    n_global = n;
    vector<int> original(n);
    
    for (int idx = 0; idx < n; idx++) {
        cin >> original[idx];
    }

    vector<int> arr_rand = original;
    vector<int> arr_sorted = original;
    sort(arr_sorted.begin(), arr_sorted.end()); 

    if (n <= 30) {
        cout << "Poczatkowy stan tablicy:         "; printArray(original);    }
    cout<<endl;
    int result = randomSelect(arr_rand, 0, n - 1, k);
    
    if (n <= 30) {
        cout << "Koncowy stan tablicy:            "; printArray(arr_rand);
        cout << "Znaleziona statystyka pozycyjna: " << result << endl;
        cout << "Posortowany ciag kluczy:         "; printArray(arr_sorted);
    }

    cout << "Rozmiar danych (n): " << n << endl;
    cout << "Szukana statystyka pozycyjna (k): " << k << endl;
    cout << "Liczba porownan:    " << porownania << endl;
    cout << "Liczba przestawien: " << przestawienia << endl;

    return 0;
}