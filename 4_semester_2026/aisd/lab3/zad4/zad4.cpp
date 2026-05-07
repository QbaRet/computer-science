#include <iostream>
#include <vector>
#include <cmath>      
#include <random>
#include <algorithm>  
#include <fstream>   

using namespace std;
const double delta_val = 10.0; 

int licznik_porownan = 0;

int binarySearch(const vector<double>& arr, double value, int left, int right){
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
                return mid; 
            }
        }
    }
    return -1; 
}

double f(int x){
    return pow(log2(x + 1), 3);
}

double inverseFunction(double y){
    return exp2(cbrt(y)) - 1.0;
}

int smartSearch(const vector<double>& arr, double value){
    double val_left = max(0.0, value - delta_val);
    double val_right = value;

    int left_index = floor(inverseFunction(val_left));
    int right_index = ceil(inverseFunction(val_right));

    int n = arr.size();
    left_index = max(0, left_index);
    right_index = min(n - 1, right_index);

    if (left_index > right_index) {
        return -1;
    }

    return binarySearch(arr, value, left_index, right_index);
}

int main(){
    ofstream file("wyniki_zad4.csv");
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
        
        if(n % 50000 == 0) cout << "Przetwarzam n = " << n << "..." << endl;
    }

    file.close();
    cout << "Gotowe! Wyniki zapisane do pliku 'wyniki_zad4.csv'." << endl;

    return 0;
}