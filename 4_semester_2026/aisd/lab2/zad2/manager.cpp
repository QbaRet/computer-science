#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

void run_test(string algo, int n, int k, string filename) {
    
    string program_name;
    if (algo == "ins") program_name = "./insertion_sort";
    else if (algo == "qs") program_name = "./quick_sort";
    else if (algo == "hybrid") program_name = "./hybrid_qs_is";

    for (int i = 0; i < k; i++) {
        
        
        string cmd = "./data_generator " + to_string(n) + " | " + program_name + " " + filename;
        
        system(cmd.c_str());
    }
}

int main() {
    
    system("g++ -O3 data_generator.cpp -o data_generator");
    system("g++ -O3 insertion_sort.cpp -o insertion_sort");
    system("g++ -O3 quick_sort.cpp -o quick_sort");
    system("g++ -O3 hybrid_qs_is.cpp -o hybrid_qs_is");

    int k_options[] = {1, 10, 100};

    for (int k : k_options) {
        string fname = "wyniki_k" + to_string(k) + ".csv";

        
        for (int n = 10; n <= 50; n += 10) {
            run_test("ins", n, k, fname);
            run_test("qs", n, k, fname);
            run_test("hybrid", n, k, fname);
        }

        
        for (int n = 1000; n <= 50000; n += 1000) {
            run_test("qs", n, k, fname);
            run_test("hybrid", n, k, fname);
            if(n % 10000 == 0) cout << "Postep: k=" << k << ", n=" << n << "..." << endl;
        }
    }
    return 0;
}