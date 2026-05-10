#include<iostream>
#include<vector>
#include<cmath>
#include<iomanip>
#include<algorithm>

using namespace std;

long long porownania = 0;
long long przestawienia = 0;
long long licznik_partycji = 0; 
int n_global = 0;

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

int partition(vector<int>& arr, int left, int right, int x){
    int index=-1;
    for(int j=left; j<= right ; j++){
        if(arr[j]==x){ 
            index=j;
            break;
        }
    }
    int temp = arr[index];
    set_element(arr[index], arr[right]);
    set_element(arr[right], temp);
    
    int i=left; 
    for(int j=left; j<=right-1; j++){
        if(!compare(arr[j], x) || arr[j]==x){
            int tmp = arr[i];
            set_element(arr[i], arr[j]);
            set_element(arr[j], tmp);
            i++;
        }
    }
    int tmp = arr[i];
    set_element(arr[i], arr[right]);
    set_element(arr[right], tmp);
    
    if (n_global <= 30) {
        cout << "Po partycji " << ++licznik_partycji << " (pivot: " << setw(2) << setfill('0') << x << "): ";
        printArray(arr);
    }
    
    return i;
}

void insertionSort(vector<int>& arr, int l, int r){
    for(int i=l+1; i<=r; i++){
        int key=arr[i];
        int j=i-1;
        while(j>=l && compare(arr[j], key)){ 
            set_element(arr[j+1], arr[j]);
            j--;
        }
        set_element(arr[j+1], key);
    }
}

int select_algo(vector<int>& arr, int left, int right, int i){ 
    if(left >= right) return arr[left];
    int n = right-left+1;
    int x;
    
    int number_of_groups = ceil(n / 5.0); 
    vector<int>M(number_of_groups);

    for(int j=0; j<number_of_groups; j++){
        int start= left + j*5;
        int end = min(left + j*5 + 4, right);
        insertionSort(arr, start, end);

        int mid = start + (end - start)/2;
        M[j]=arr[mid];
    }
    
    if(number_of_groups==1) {
        x=M[0];
    } else {
        int median_index=floor(number_of_groups/2)+1;
        x=select_algo(M,0,number_of_groups-1,median_index); 
    }
    int index_x=partition(arr,left,right,x);
    int k=index_x-left+1;

    if(i==k){
        return x;
    }
    if(i<k){
        return select_algo(arr, left, index_x - 1, i);
    } else {
        return select_algo(arr, index_x + 1, right, i - k); 
    }
}

int main() {
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

    vector<int> arr_sel = original;
    vector<int> arr_sorted = original;
    sort(arr_sorted.begin(), arr_sorted.end()); 

    if (n <= 30) {
        cout << "Poczatkowy stan tablicy:         "; printArray(original);
    }

    int result_sel = select_algo(arr_sel, 0, n-1, k);

    if (n <= 30) {

        cout << "Koncowy stan tablicy:            "; printArray(arr_sel);
        cout << "Znaleziona statystyka pozycyjna: " << result_sel << endl;
        cout << "Posortowany ciag kluczy:         "; printArray(arr_sorted);
    }
    
    cout << "Rozmiar danych (n): " << n << endl;
    cout << "Szukana statystyka pozycyjna (k): " << k << endl;
    cout << "Liczba porownan:    " << porownania << endl;
    cout << "Liczba przestawien: " << przestawienia << endl;

    return 0;
}