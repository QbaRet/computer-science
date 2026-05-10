#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>

using namespace std;

long long porownania = 0;
long long przestawienia = 0;

bool compare_val(int a, int b) {
    porownania++;
    return a > b; 
}

bool compare_val_rand(int a, int b) {
    porownania++;
    return a <= b; 
}

void set_element(int &dest, int value) {
    przestawienia++;
    dest = value;
}

void custom_swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
    przestawienia += 2;
}

int partition_select(vector<int>& arr, int left, int right, int x){
    int index=-1;
    for(int j=left; j<= right ; j++){
        if(arr[j]==x){ index=j; break; }
    }
    int temp = arr[index];
    set_element(arr[index], arr[right]);
    set_element(arr[right], temp);
    
    int i=left; 
    for(int j=left; j<=right-1; j++){
        if(!compare_val(arr[j], x) || arr[j]==x){
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
    for(int i=l+1; i<=r; i++){
        int key=arr[i];
        int j=i-1;
        while(j>=l && compare_val(arr[j], key)){ 
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
    
    if(number_of_groups==1) { x=M[0]; } 
    else {
        int median_index=floor(number_of_groups/2)+1;
        x=select_algo(M,0,number_of_groups-1,median_index);
    }
    
    int index_x=partition_select(arr,left,right,x);
    int k=index_x-left+1;

    if(i==k) return x;
    if(i<k) return select_algo(arr, left, index_x - 1, i);
    else return select_algo(arr, index_x + 1, right, i - k); 
}


int randomPartition(vector<int>& arr, int left, int right){
    int random_index = left + rand() % (right - left + 1);
    custom_swap(arr[random_index], arr[right]);
    int pivot = arr[right];
    int i = left;
    for(int j = left; j < right; j++){
        if(compare_val_rand(arr[j], pivot)){
            custom_swap(arr[i], arr[j]);
            i++; 
        }
    }
    custom_swap(arr[i], arr[right]);
    return i;
}

int randomSelect(vector<int>& arr, int left, int right, int i){
    if(left >= right) return arr[left];
    int q = randomPartition(arr, left, right);
    int k = q - left + 1;
    if(i == k) return arr[q]; 
    else if(i < k) return randomSelect(arr, left, q - 1, i); 
    else return randomSelect(arr, q + 1, right, i - k);
}


int main() {
    srand(time(NULL));
    ofstream file("wyniki.csv");
    file << "n,k_typ,algorytm,avg_porownania,avg_przestawienia\n";

    int max_n = 50000;
    int step = 100;
    int m = 50; 


    for (int n = 100; n <= max_n; n += step) {
        if (n % 5000 == 0) cout << " n = " << n << endl;

        for (int k_typ = 1; k_typ <= 5; k_typ++) {
            int k_val;
            if (k_typ == 1) k_val = 1;
            else if (k_typ == 2) k_val = max(1, n / 4);
            else if (k_typ == 3) k_val = max(1, n / 2);
            else if (k_typ == 4) k_val = max(1, 3 * n / 4);
            else k_val = n;

            long long total_cmp_sel = 0, total_swp_sel = 0;
            long long total_cmp_rand = 0, total_swp_rand = 0;

            for (int rep = 0; rep < m; rep++) {
                vector<int> original(n);
                for (int i = 0; i < n; i++) {
                    original[i] = rand() % (2 * n);
                }

                vector<int> arr_sel = original;
                porownania = 0; przestawienia = 0;
                select_algo(arr_sel, 0, n - 1, k_val);
                total_cmp_sel += porownania;
                total_swp_sel += przestawienia;

                vector<int> arr_rand = original;
                porownania = 0; przestawienia = 0;
                randomSelect(arr_rand, 0, n - 1, k_val);
                total_cmp_rand += porownania;
                total_swp_rand += przestawienia;
            }

            file << n << "," << k_typ << ",SELECT," << (total_cmp_sel / m) << "," << (total_swp_sel / m) << "\n";
            file << n << "," << k_typ << ",RAND_SELECT," << (total_cmp_rand / m) << "," << (total_swp_rand / m) << "\n";
        }
    }

    file.close();
    return 0;
}