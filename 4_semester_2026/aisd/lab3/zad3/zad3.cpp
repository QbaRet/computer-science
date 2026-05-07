#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <chrono> // Do pomiaru czasu

using namespace std;
using namespace std::chrono;

long long porownania = 0;
long long przestawienia = 0;

bool compare_val(int a, int b) {
    porownania++;
    return a > b; 
}

void set_element(int &dest, int value) {
    przestawienia++;
    dest = value;
}

int partition_select(vector<int>& arr, int left, int right, int x){
    int index = -1;
    for(int j = left; j <= right; j++){
        if(arr[j] == x){ index = j; break; }
    }
    int temp = arr[index];
    set_element(arr[index], arr[right]);
    set_element(arr[right], temp);
    
    int i = left; 
    for(int j = left; j <= right - 1; j++){
        if(!compare_val(arr[j], x) || arr[j] == x){
            int tmp = arr[i];
            set_element(arr[i], arr[j]);
            set_element(arr[j], tmp);
            i++;
        }
    }
    int tmp = arr[i];
    set_element(arr[i], arr[right]);
    set_element(arr[right], tmp);
    return i;
}

void insertionSort(vector<int>& arr, int l, int r){
    for(int i = l + 1; i <= r; i++){
        int key = arr[i];
        int j = i - 1;
        while(j >= l && compare_val(arr[j], key)){ 
            set_element(arr[j+1], arr[j]);
            j--;
        }
        set_element(arr[j+1], key);
    }
}

// Zmodyfikowany algorytm - dodano parametr 'group_size'
int select_algo(vector<int>& arr, int left, int right, int i, int group_size){ 
    if(left >= right) return arr[left];
    int n = right - left + 1;
    int x;
    
    int number_of_groups = ceil((double)n / group_size); 
    vector<int> M(number_of_groups);

    for(int j = 0; j < number_of_groups; j++){
        int start = left + j * group_size;
        int end = min(left + j * group_size + (group_size - 1), right);
        insertionSort(arr, start, end);
        int mid = start + (end - start) / 2;
        M[j] = arr[mid];
    }
    
    if(number_of_groups == 1) { 
        x = M[0]; 
    } else {
        int median_index = floor(number_of_groups / 2) + 1;
        // Rekurencyjne wywolanie z tym samym rozmiarem grupy
        x = select_algo(M, 0, number_of_groups - 1, median_index, group_size);
    }
    
    int index_x = partition_select(arr, left, right, x);
    int k_stat = index_x - left + 1;

    if(i == k_stat) return x;
    if(i < k_stat) return select_algo(arr, left, index_x - 1, i, group_size);
    else return select_algo(arr, index_x + 1, right, i - k_stat, group_size); 
}

int main() {
    srand(time(NULL));
    ofstream file("wyniki_zad3.csv");
    file << "n,group_size,avg_porownania,avg_przestawienia,avg_czas_us\n";

    int max_n = 100000;
    int step = 1000; // Zwiekszamy krok do 1000, zeby wykresy byly plynne, a testy szybsze
    int m = 50; 
    vector<int> group_sizes = {3, 5, 7, 9, 19, 21};

    cout << "Rozpoczynam testy do Zadania 3..." << endl;

    for (int n = 1000; n <= max_n; n += step) {
        if (n % 5000 == 0) cout << "Przetwarzam n = " << n << "..." << endl;

        int k_statystyka = n / 2; // Zawsze szukamy mediany (najlepszy test dla SELECT)

        for (int group_size : group_sizes) {
            long long total_cmp = 0, total_swp = 0;
            long long total_time_us = 0;

            for (int rep = 0; rep < m; rep++) {
                vector<int> arr(n);
                for (int i = 0; i < n; i++) {
                    arr[i] = rand() % (2 * n);
                }

                porownania = 0; przestawienia = 0;
                
                // Mierzymy czas dla konkretnego wywolania
                auto start_time = high_resolution_clock::now();
                select_algo(arr, 0, n - 1, k_statystyka, group_size);
                auto end_time = high_resolution_clock::now();
                
                auto duration = duration_cast<microseconds>(end_time - start_time);
                
                total_cmp += porownania;
                total_swp += przestawienia;
                total_time_us += duration.count();
            }

            file << n << "," << group_size << "," 
                 << (total_cmp / m) << "," 
                 << (total_swp / m) << "," 
                 << (total_time_us / m) << "\n";
        }
    }

    file.close();
    cout << "Zakonczono! Wyniki zapisano do 'wyniki_zad3.csv'." << endl;
    return 0;
}