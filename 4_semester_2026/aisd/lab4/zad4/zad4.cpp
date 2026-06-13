#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

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

Node* rightRotate(Node* x) {
    ptr_count += 3;
    Node* y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

Node* leftRotate(Node* x) {
    ptr_count += 3;
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

Node* splay(Node* root, int val) {
    ptr_count++;
    if (root == nullptr) {
        return nullptr;
    }

    Node header(0);
    header.left = nullptr;
    header.right = nullptr;
    Node* leftTreeMax = &header;
    Node* rightTreeMin = &header;

    while (true) {
        cmp_count++;
        if (val < root->data) {
            ptr_count++;
            if (root->left == nullptr) {
                break;
            }

            cmp_count++;
            ptr_count++;
            if (val < root->left->data) {
                root = rightRotate(root);
                ptr_count++;
                if (root->left == nullptr) {
                    break;
                }
            }

            ptr_count += 2;
            rightTreeMin->left = root;
            rightTreeMin = rightTreeMin->left;
            ptr_count += 2;
            root = root->left;
        } else {
            cmp_count++;
            if (val > root->data) {
                ptr_count++;
                if (root->right == nullptr) {
                    break;
                }

                cmp_count++;
                ptr_count++;
                if (val > root->right->data) {
                    root = leftRotate(root);
                    ptr_count++;
                    if (root->right == nullptr) {
                        break;
                    }
                }

                ptr_count += 2;
                leftTreeMax->right = root;
                leftTreeMax = leftTreeMax->right;
                ptr_count += 2;
                root = root->right;
            } else {
                break;
            }
        }
    }

    ptr_count += 4;
    leftTreeMax->right = root->left;
    rightTreeMin->left = root->right;
    root->left = header.right;
    root->right = header.left;

    return root;
}

Node* insert(Node* root, int val) {
    ptr_count++;
    if (root == nullptr) {
        return new Node(val);
    }

    root = splay(root, val);
    cmp_count++;
    if (root->data == val) {
        return root;
    }

    Node* newNode = new Node(val);
    cmp_count++;
    if (val < root->data) {
        ptr_count += 4;
        newNode->left = root->left;
        newNode->right = root;
        root->left = nullptr;
    } else {
        ptr_count += 4;
        newNode->right = root->right;
        newNode->left = root;
        root->right = nullptr;
    }

    return newNode;
}

Node* deleteNode(Node* root, int val) {
    ptr_count++;
    if (root == nullptr) {
        return nullptr;
    }

    root = splay(root, val);
    cmp_count++;
    if (root->data != val) {
        return root;
    }

    Node* temp;
    ptr_count++;
    if (root->left == nullptr) {
        temp = root;
        root = root->right;
    } else {
        temp = root;
        root = splay(root->left, val);
        ptr_count += 2;
        root->right = temp->right;
    }

    delete temp;
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
    ofstream out("wyniki4.csv");
    out << "N,Scenario,Operation,AvgCmp,MaxCmp,AvgPtr,MaxPtr,AvgHeight,MaxHeight\n";

    mt19937 rng(1337);
    const int NUM_TESTS = 20;

    vector<int> N_values;
    for (int i = 10000; i <= 100000; i += 10000) {
        N_values.push_back(i);
    }

    for (int n : N_values) {
        cout << "Przeprowadzanie testow dla N = " << n << "..." << endl;

        long long s1_ins_cmp_sum = 0, s1_ins_cmp_max = 0, s1_ins_ptr_sum = 0, s1_ins_ptr_max = 0, s1_ins_h_sum = 0, s1_ins_h_max = 0;
        long long s1_del_cmp_sum = 0, s1_del_cmp_max = 0, s1_del_ptr_sum = 0, s1_del_ptr_max = 0, s1_del_h_sum = 0, s1_del_h_max = 0;

        long long s2_ins_cmp_sum = 0, s2_ins_cmp_max = 0, s2_ins_ptr_sum = 0, s2_ins_ptr_max = 0, s2_ins_h_sum = 0, s2_ins_h_max = 0;
        long long s2_del_cmp_sum = 0, s2_del_cmp_max = 0, s2_del_ptr_sum = 0, s2_del_ptr_max = 0, s2_del_h_sum = 0, s2_del_h_max = 0;

        for (int t = 0; t < NUM_TESTS; t++) {
            vector<int> seq_inc(n);
            iota(seq_inc.begin(), seq_inc.end(), 1);

            vector<int> seq_rand = seq_inc;
            shuffle(seq_rand.begin(), seq_rand.end(), rng);

            vector<int> del_rand1 = seq_inc;
            shuffle(del_rand1.begin(), del_rand1.end(), rng);

            vector<int> del_rand2 = seq_inc;
            shuffle(del_rand2.begin(), del_rand2.end(), rng);

            Node* root1 = nullptr;
            int current_h = 0;

            for (int val : seq_inc) {
                reset_counters();
                root1 = insert(root1, val);

                s1_ins_cmp_sum += cmp_count;
                s1_ins_cmp_max = max(s1_ins_cmp_max, cmp_count);
                s1_ins_ptr_sum += ptr_count;
                s1_ins_ptr_max = max(s1_ins_ptr_max, ptr_count);

                current_h = iterative_height(root1);
                s1_ins_h_sum += current_h;
                s1_ins_h_max = max(s1_ins_h_max, (long long)current_h);
            }

            for (int val : del_rand1) {
                reset_counters();
                root1 = deleteNode(root1, val);

                s1_del_cmp_sum += cmp_count;
                s1_del_cmp_max = max(s1_del_cmp_max, cmp_count);
                s1_del_ptr_sum += ptr_count;
                s1_del_ptr_max = max(s1_del_ptr_max, ptr_count);

                current_h = iterative_height(root1);
                s1_del_h_sum += current_h;
                s1_del_h_max = max(s1_del_h_max, (long long)current_h);
            }

            Node* root2 = nullptr;
            current_h = 0;

            for (int val : seq_rand) {
                reset_counters();
                root2 = insert(root2, val);

                s2_ins_cmp_sum += cmp_count;
                s2_ins_cmp_max = max(s2_ins_cmp_max, cmp_count);
                s2_ins_ptr_sum += ptr_count;
                s2_ins_ptr_max = max(s2_ins_ptr_max, ptr_count);

                current_h = iterative_height(root2);
                s2_ins_h_sum += current_h;
                s2_ins_h_max = max(s2_ins_h_max, (long long)current_h);
            }

            for (int val : del_rand2) {
                reset_counters();
                root2 = deleteNode(root2, val);

                s2_del_cmp_sum += cmp_count;
                s2_del_cmp_max = max(s2_del_cmp_max, cmp_count);
                s2_del_ptr_sum += ptr_count;
                s2_del_ptr_max = max(s2_del_ptr_max, ptr_count);

                current_h = iterative_height(root2);
                s2_del_h_sum += current_h;
                s2_del_h_max = max(s2_del_h_max, (long long)current_h);
            }
        }

        double total_ops = (double)NUM_TESTS * n;

        out << n << ",1,Insert," << s1_ins_cmp_sum / total_ops << "," << s1_ins_cmp_max << ","
            << s1_ins_ptr_sum / total_ops << "," << s1_ins_ptr_max << ","
            << s1_ins_h_sum / total_ops << "," << s1_ins_h_max << "\n";

        out << n << ",1,Delete," << s1_del_cmp_sum / total_ops << "," << s1_del_cmp_max << ","
            << s1_del_ptr_sum / total_ops << "," << s1_del_ptr_max << ","
            << s1_del_h_sum / total_ops << "," << s1_del_h_max << "\n";

        out << n << ",2,Insert," << s2_ins_cmp_sum / total_ops << "," << s2_ins_cmp_max << ","
            << s2_ins_ptr_sum / total_ops << "," << s2_ins_ptr_max << ","
            << s2_ins_h_sum / total_ops << "," << s2_ins_h_max << "\n";

        out << n << ",2,Delete," << s2_del_cmp_sum / total_ops << "," << s2_del_cmp_max << ","
            << s2_del_ptr_sum / total_ops << "," << s2_del_ptr_max << ","
            << s2_del_h_sum / total_ops << "," << s2_del_h_max << "\n";
    }

    return 0;
}
