#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>

using namespace std;

char left_trace[100];
char right_trace[100];

struct Node {
    int data;
    Node* right;
    Node* left;

    Node(int data) {
        this->data = data;
        this->right = nullptr;
        this->left = nullptr;
    }
};

void clear_traces() {
    fill_n(left_trace, 100, ' ');
    fill_n(right_trace, 100, ' ');
}

int tree_height(Node* root) {
    if (root == nullptr) return 0;
    return 1 + max(tree_height(root->left), tree_height(root->right));
}

Node* insert(Node* root, int val) {
    if (root == nullptr) return new Node(val);

    Node* current = root;
    Node* parent = nullptr;

    while (current != nullptr) {
        parent = current;
        if (val < current->data) {
            current = current->left;
        } else if (val > current->data) {
            current = current->right;
        } else {
            return root; 
        }
    }

    if (val < parent->data) {
        parent->left = new Node(val);
    } else {
        parent->right = new Node(val);
    }

    return root;
}

Node* deleteNode(Node* root, int val) {
    Node* curr = root;
    Node* parent = nullptr;

    while (curr != nullptr && curr->data != val) {
        parent = curr;
        if (val < curr->data) curr = curr->left;
        else curr = curr->right;
    }

    if (curr == nullptr) return root;

    if (curr->left == nullptr || curr->right == nullptr) {
        Node* newCurr = (curr->left == nullptr) ? curr->right : curr->left;

        if (parent == nullptr) {
            delete curr;
            return newCurr;
        }

        if (curr == parent->left) parent->left = newCurr;
        else parent->right = newCurr;
        
        delete curr;
    } 
    else {
        Node* p = nullptr;
        Node* temp = curr->right;
        
        while (temp->left != nullptr) {
            p = temp;
            temp = temp->left;
        }

        if (p != nullptr) p->left = temp->right;
        else curr->right = temp->right;

        curr->data = temp->data; 
        delete temp;
    }
    return root;
}

void print_BST(Node* root, int depth = 0, char prefix = '-') {
    if (root == nullptr) return;

    if (root->left != nullptr) {
        print_BST(root->left, depth + 1, '/');
    }

    if (prefix == '/') left_trace[depth - 1] = '|';
    if (prefix == '\\') right_trace[depth - 1] = ' ';

    if (depth == 0) cout << "-";
    if (depth > 0) cout << " ";

    for (int i = 0; i < depth - 1; i++) {
        if (left_trace[i] == '|' || right_trace[i] == '|') {
            cout << "| ";
        } else {
            cout << "  ";
        }
    }

    if (depth > 0) cout << prefix << "-";
    cout << "[" << root->data << "]" << endl;

    left_trace[depth] = ' ';
    if (root->right != nullptr) {
        right_trace[depth] = '|';
        print_BST(root->right, depth + 1, '\\');
    }
}

int main() {
    int N = 30;
    mt19937 rng(1337);

    vector<int> seq_inc(N);
    iota(seq_inc.begin(), seq_inc.end(), 1); 
    
    vector<int> seq_rand = seq_inc;
    shuffle(seq_rand.begin(), seq_rand.end(), rng);

    auto show_tree = [](Node* root) {
        clear_traces();
        print_BST(root);
        cout << "Wysokosc drzewa: " << tree_height(root) << endl;
        cout << "-----------------------------------" << endl;
    };

    cout << "1: Wstawianie rosnace:" << endl;
    Node* root1 = nullptr;
    for (int val : seq_inc) {
        cout << "\ninsert " << val << endl;
        root1 = insert(root1, val);
        show_tree(root1);
    }

    cout << "\n1: Usuwanie :" << endl;
    vector<int> del_rand1 = seq_inc;
    shuffle(del_rand1.begin(), del_rand1.end(), rng);
    for (int val : del_rand1) {
        cout << "\ndelete " << val << endl;
        root1 = deleteNode(root1, val);
        show_tree(root1);
    }

    cout << "\n2: Wstawianie losowe " << endl;
    Node* root2 = nullptr;
    for (int val : seq_rand) {
        cout << "\ninsert " << val << endl;
        root2 = insert(root2, val);
        show_tree(root2);
    }

    cout << "\n2: Usuwanie losowe" << endl;
    vector<int> del_rand2 = seq_inc;
    shuffle(del_rand2.begin(), del_rand2.end(), rng);
    for (int val : del_rand2) {
        cout << "\ndelete " << val << endl;
        root2 = deleteNode(root2, val);
        show_tree(root2);
    }

    return 0;
}