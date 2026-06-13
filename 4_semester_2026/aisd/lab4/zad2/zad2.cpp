#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <fstream>
#include <chrono>

using namespace std;

long long cmp_count = 0;
long long ptr_count = 0;

void reset_counters() {
    cmp_count = 0;
    ptr_count = 0;
}

struct Node {
    int data;
    Node* right;
    Node* left;

    Node(int data) {
        this->data = data;
        this->right = nullptr;
        this->left = nullptr;
        ptr_count += 2; 
    }
};

// --- WERSJA ITERACYJNA INSERT ---
Node* insert(Node* root, int val, int& final_depth) {
    if (root == nullptr) {
        final_depth = 1;
        return new Node(val);
    }

    Node* current = root;
    Node* parent = nullptr;
    int depth = 1;

    while (current != nullptr) {
        parent = current;
        cmp_count++;
        if (val < current->data) {
            ptr_count += 2; 
            current = current->left;
        } else if (val > current->data) {
            cmp_count++;
            ptr_count += 2;
            current = current->right;
        } else {
            final_depth = depth;
            return root;
        }
        depth++;
    }

    final_depth = depth;
    Node* newNode = new Node(val);
    if (val < parent->data) {
        parent->left = newNode;
        ptr_count++;
    } else {
        parent->right = newNode;
        ptr_count++;
    }
    return root;
}

// --- WERSJA ITERACYJNA DELETE ---
Node* deleteNode(Node* root, int val) {
    Node *curr = root, *parent = nullptr;

    while (curr != nullptr && curr->data != val) {
        parent = curr;
        cmp_count++;
        if (val < curr->data) curr = curr->left;
        else curr = curr->right;
        ptr_count++;
    }

    if (curr == nullptr) return root;

    if (curr->left == nullptr || curr->right == nullptr) {
        Node* newCurr = (curr->left == nullptr) ? curr->right : curr->left;
        if (parent == nullptr) { delete curr; return newCurr; }
        if (curr == parent->left) parent->left = newCurr;
        else parent->right = newCurr;
        delete curr;
    } else {
        Node* p = nullptr;
        Node* temp = curr->right;
        while (temp->left != nullptr) {
            p = temp;
            temp = temp->left;
            ptr_count++;
        }
        if (p != nullptr) p->left = temp->right;
        else curr->right = temp->right;
        
        curr->data = temp->data;
        delete temp;
    }
    return root;
}

int iterative_height(Node* root) {
    if (!root) return 0;
    int max_h = 0;
    vector<pair<Node*, int>> stack;
    stack.push_back({root, 1});
    
    while (!stack.empty()) {
        auto curr = stack.back();
        stack.pop_back();
        Node* node = curr.first;
        int h = curr.second;
        
        if (h > max_h) max_h = h;
        if (node->left) stack.push_back({node->left, h + 1});
        if (node->right) stack.push_back({node->right, h + 1});
    }
    return max_h;
}

int main() {
    ofstream out("wyniki2.csv");
    out << "N,Scenario,Operation,AvgCmp,MaxCmp,AvgPtr,MaxPtr,AvgHeight,MaxHeight\n";
    
    mt19937 rng(1337); 
    const int NUM_TESTS = 20;
    vector<int> N_values = {10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};

    for (int n : N_values) {
        cout << "Przeprowadzanie testow dla N = " << n << "..." << endl;
        // ... (Logika sumowania zmiennych pozostaje bez zmian) ...
        long long s1_ins_cmp_sum = 0, s1_ins_cmp_max = 0, s1_ins_ptr_sum = 0, s1_ins_ptr_max = 0, s1_ins_h_sum = 0, s1_ins_h_max = 0;
        long long s1_del_cmp_sum = 0, s1_del_cmp_max = 0, s1_del_ptr_sum = 0, s1_del_ptr_max = 0, s1_del_h_sum = 0, s1_del_h_max = 0;
        long long s2_ins_cmp_sum = 0, s2_ins_cmp_max = 0, s2_ins_ptr_sum = 0, s2_ins_ptr_max = 0, s2_ins_h_sum = 0, s2_ins_h_max = 0;
        long long s2_del_cmp_sum = 0, s2_del_cmp_max = 0, s2_del_ptr_sum = 0, s2_del_ptr_max = 0, s2_del_h_sum = 0, s2_del_h_max = 0;

        for (int t = 0; t < NUM_TESTS; t++) {
            vector<int> seq_inc(n); iota(seq_inc.begin(), seq_inc.end(), 1);
            vector<int> seq_rand = seq_inc; shuffle(seq_rand.begin(), seq_rand.end(), rng);
            vector<int> del_rand1 = seq_inc; shuffle(del_rand1.begin(), del_rand1.end(), rng);
            vector<int> del_rand2 = seq_inc; shuffle(del_rand2.begin(), del_rand2.end(), rng);

            // Scenariusz 1
            Node* root1 = nullptr; int current_h = 0;
            for (int val : seq_inc) {
                reset_counters(); int final_depth = 1;
                root1 = insert(root1, val, final_depth);
                s1_ins_cmp_sum += cmp_count; s1_ins_cmp_max = max(s1_ins_cmp_max, cmp_count);
                s1_ins_ptr_sum += ptr_count; s1_ins_ptr_max = max(s1_ins_ptr_max, ptr_count);
                if (final_depth > current_h) current_h = final_depth;
                s1_ins_h_sum += current_h; s1_ins_h_max = max(s1_ins_h_max, (long long)current_h);
            }
            for (int val : del_rand1) {
                reset_counters(); root1 = deleteNode(root1, val);
                s1_del_cmp_sum += cmp_count; s1_del_cmp_max = max(s1_del_cmp_max, cmp_count);
                s1_del_ptr_sum += ptr_count; s1_del_ptr_max = max(s1_del_ptr_max, ptr_count);
                current_h = iterative_height(root1);
                s1_del_h_sum += current_h; s1_del_h_max = max(s1_del_h_max, (long long)current_h);
            }

            // Scenariusz 2
            Node* root2 = nullptr; current_h = 0;
            for (int val : seq_rand) {
                reset_counters(); int final_depth = 1;
                root2 = insert(root2, val, final_depth);
                s2_ins_cmp_sum += cmp_count; s2_ins_cmp_max = max(s2_ins_cmp_max, cmp_count);
                s2_ins_ptr_sum += ptr_count; s2_ins_ptr_max = max(s2_ins_ptr_max, ptr_count);
                if (final_depth > current_h) current_h = final_depth;
                s2_ins_h_sum += current_h; s2_ins_h_max = max(s2_ins_h_max, (long long)current_h);
            }
            for (int val : del_rand2) {
                reset_counters(); root2 = deleteNode(root2, val);
                s2_del_cmp_sum += cmp_count; s2_del_cmp_max = max(s2_del_cmp_max, cmp_count);
                s2_del_ptr_sum += ptr_count; s2_del_ptr_max = max(s2_del_ptr_max, ptr_count);
                current_h = iterative_height(root2);
                s2_del_h_sum += current_h; s2_del_h_max = max(s2_del_h_max, (long long)current_h);
            }
        }
        // ... (Zapis do pliku CSV bez zmian) ...
        double total_ops = (double)NUM_TESTS * n;
        out << n << ",1,Insert," << s1_ins_cmp_sum/total_ops << "," << s1_ins_cmp_max << "," << s1_ins_ptr_sum/total_ops << "," << s1_ins_ptr_max << "," << s1_ins_h_sum/total_ops << "," << s1_ins_h_max << "\n";
        out << n << ",1,Delete," << s1_del_cmp_sum/total_ops << "," << s1_del_cmp_max << "," << s1_del_ptr_sum/total_ops << "," << s1_del_ptr_max << "," << s1_del_h_sum/total_ops << "," << s1_del_h_max << "\n";
        out << n << ",2,Insert," << s2_ins_cmp_sum/total_ops << "," << s2_ins_cmp_max << "," << s2_ins_ptr_sum/total_ops << "," << s2_ins_ptr_max << "," << s2_ins_h_sum/total_ops << "," << s2_ins_h_max << "\n";
        out << n << ",2,Delete," << s2_del_cmp_sum/total_ops << "," << s2_del_cmp_max << "," << s2_del_ptr_sum/total_ops << "," << s2_del_ptr_max << "," << s2_del_h_sum/total_ops << "," << s2_del_h_max << "\n";
    }
    cout << "Zakonczono! Wyniki zapisano do pliku wyniki2.csv" << endl;
    return 0;
}