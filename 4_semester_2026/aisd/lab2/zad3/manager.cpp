#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;
void run_test(string algo, int n, int k, string filename) {
    string program_name;
    
    if (algo == "sps") program_name = "./my_sort"; 
    else if (algo == "ms") program_name = "./merge_sort";     
    
    for (int i = 0; i < k; i++) {
        
        string cmd = "./data_generator " + to_string(n) + " | " + program_name + " " + filename;
        system(cmd.c_str());
    }
}

int main() {
    system("g++ -O3 data_generator.cpp -o data_generator");
    system("g++ -O3 my_sort.cpp -o my_sort");
    system("g++ -O3 merge_sort.cpp -o merge_sort");
    
    int k_options[] = {1, 10, 100};

    for (int k : k_options) {
        
        string fname = "wyniki_zad3_k" + to_string(k) + ".csv";
        for (int n = 1000; n <= 50000; n += 1000) {
            run_test("sps", n, k, fname);
            run_test("ms", n, k, fname);
        }
    }
    return 0;
}