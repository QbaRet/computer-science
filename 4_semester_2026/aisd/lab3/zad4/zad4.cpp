#include <iostream>
#include <vector>
#include <cmath>      
#include <random>
#include <algorithm>  
#include <fstream>   
#include <iomanip>

using namespace std;

const double delta_val = 50.0; 

int licznik_porownan = 0;

int binarySearch(const vector<double>& arr, double value, int left, int right, bool verbose = false){
    int mid;
    while(left <= right){
        licznik_porownan++; 
        mid = left + (right - left) / 2; 
        
        if(arr[mid] > value){
            right = mid - 1;
        }
        else {
            licznik_porownan++; 
            if(arr[mid] < value) {
                left = mid + 1;
            }
            else {
                if (verbose) {
                    cout << "Zakonczono wyszukiwanie. Indeksy na ktorych sie zatrzymano: left = " << left 
                         << ", right = " << right << ", znaleziono na: " << mid << "\n";
                }
                return mid; 
            }
        }
    }
    if (verbose) {
        cout << "Zakonczono wyszukiwanie. Indeksy na ktorych sie zatrzymano: left = " << left 
             << ", right = " << right << " (nie znaleziono)\n";
    }
    return -1; 
}

double f(double x){
    return sqrt(x) + log2(x + 1.0);
}

double derivative_f(double x) {
    if (x <= 0.0) return 1e9; 
    return 1.0 / (2.0 * sqrt(x)) + 1.0 / ((x + 1.0) * log(2.0));
}

double inverseFunction(double y){
    if (y <= 0.0) return 0.0;

    double x = y * y; 
    
    for (int i = 0; i < 6; ++i) {
        double f_val = f(x);
        double diff = f_val - y;
        
        if (abs(diff) < 1e-4) break; 
        
        x = x - diff / derivative_f(x);
        if (x < 0) x = 0;
    }
    return x;
}

int smartSearch(const vector<double>& arr, double value, bool verbose = false){
    double val_left = max(0.0, value - delta_val);
    double val_right = value;

    int left_index = floor(inverseFunction(val_left)) - 2;
    int right_index = ceil(inverseFunction(val_right)) + 2;

    int n = arr.size();
    left_index = max(0, left_index);
    right_index = min(n - 1, right_index);

    if (verbose) {
        cout << "Oszacowany przedzial poczatkowy (Smart Search): [" << left_index << ", " << right_index << "]\n";
    }

    if (left_index > right_index) {
        if (verbose) cout << "Zakonczono: Przedzial nieprawidlowy (left > right).\n";
        return -1;
    }

    return binarySearch(arr, value, left_index, right_index, verbose);
}

void test_dla_malych_danych() {
    int n = 20;
    
    vector<double> A(n);
    random_device rd;
    mt19937_64 gen(rd()); 

    for(int i = 0; i < n; i++) {
        double min_val = f(i);
        double max_val = f(i) + delta_val;
        uniform_real_distribution<double> dis(min_val, max_val);
        A[i] = dis(gen);
    }
    sort(A.begin(), A.end());

    cout << "Input (tablica A):\n";
    for(int i = 0; i < n; ++i) {
        cout << "A[" << setw(2) << i << "] = " << fixed << setprecision(4) << A[i] << "\n";
    }


    uniform_int_distribution<int> dis_idx(0, n - 1);
    double q1 = A[dis_idx(gen)]; 

    uniform_real_distribution<double> dis_q(A[0], A[n-1]);
    double q2 = dis_q(gen); 
    while(binary_search(A.begin(), A.end(), q2)) {
        q2 = dis_q(gen);
    }

    cout << "Wyszukiwana wartosc q: " << q1 << "\n\n";
    licznik_porownan = 0;
    int wynik1 = smartSearch(A, q1, true);
    cout << "\nOdpowiedz: ";
    if (wynik1 != -1) {
        cout << "TAK (na indeksie " << wynik1 << ")\n";
    } else {
        cout << "NIE\n";
    }


    cout << "Wyszukiwana wartosc q: " << q2 << "\n\n";
    licznik_porownan = 0;
    int wynik2 = smartSearch(A, q2, true);
    cout << "\nOdpowiedz: ";
    if (wynik2 != -1) {
        cout << "TAK (na indeksie " << wynik2 << ")\n";
    } else {
        cout << "NIE\n";
    }
}

int main(){
    test_dla_malych_danych();

    ofstream file("wyniki_zad4b.csv");
    file << "n,algorytm,porownania\n";

    random_device rd;
    mt19937_64 gen(rd()); 
    for(int n = 1000; n <= 300000; n += 1000) {
        
        vector<double> A(n);

        for(int i = 0; i < n; i++) {
            double min_val = f(i);
            double max_val = f(i) + delta_val;
            
            uniform_real_distribution<double> dis(min_val, max_val);
            A[i] = dis(gen);
        }
        sort(A.begin(), A.end());

        uniform_int_distribution<int> dis_idx(0, n - 1);
        double q = A[dis_idx(gen)];
    
        licznik_porownan = 0;
        binarySearch(A, q, 0, n - 1);
        int bs_wynik = licznik_porownan;

        licznik_porownan = 0;
        smartSearch(A, q);
        int ss_wynik = licznik_porownan;

        file << n << ",BinarySearch," << bs_wynik << "\n";
        file << n << ",SmartSearch," << ss_wynik << "\n";
        

    }

    file.close();


    return 0;
}