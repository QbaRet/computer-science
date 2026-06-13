#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
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

Node* rightRotate(Node* x) {
    Node* y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

Node* leftRotate(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

Node* splay(Node* root, int val) {
    if (root == nullptr) return nullptr;

    Node header(0); 
    header.left = header.right = nullptr;
    Node* LeftTreeMax = &header;
    Node* RightTreeMin = &header;

    while (true) {
        if (val < root->data) {
            if (root->left == nullptr) break;
            if (val < root->left->data) {
                root = rightRotate(root);
                if (root->left == nullptr) break;
            }
            RightTreeMin->left = root;
            RightTreeMin = RightTreeMin->left;
            root = root->left;
        } 
        else if (val > root->data) {
            if (root->right == nullptr) break;
            if (val > root->right->data) {
                root = leftRotate(root);
                if (root->right == nullptr) break;
            }
            LeftTreeMax->right = root;
            LeftTreeMax = LeftTreeMax->right;
            root = root->right;
        } 
        else {
            break; 
        }
    }

    LeftTreeMax->right = root->left;
    RightTreeMin->left = root->right;
    root->left = header.right;
    root->right = header.left;

    return root;
}

Node* insert(Node* root, int val) {
    if (root == nullptr) return new Node(val);

    root = splay(root, val);

    if (root->data == val) return root; 

    Node* newNode = new Node(val);

    if (root->data > val) {
        newNode->right = root;
        newNode->left = root->left;
        root->left = nullptr;
    }
    else {
        newNode->left = root;
        newNode->right = root->right;
        root->right = nullptr;
    }

    return newNode;
}

Node* deleteNode(Node* root, int val) {
    if (root == nullptr) return nullptr;

    root = splay(root, val);

    if (root->data != val) return root; 

    Node* temp;
    if (root->left == nullptr) {
        temp = root;
        root = root->right;
    } else {
        temp = root;
        root = splay(root->left, val); 
        root->right = temp->right;
    }
    
    delete temp;
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

void show_tree(Node* root) {
    clear_traces();
    print_BST(root);
    cout << "Wysokosc drzewa: " << iterative_height(root) << endl;
    cout << "---------------------------------" << endl;
}

int main() {
    const int N = 30;
    mt19937 rng(1337);

    vector<int> seq_inc(N);
    iota(seq_inc.begin(), seq_inc.end(), 1);

    vector<int> seq_rand = seq_inc;
    shuffle(seq_rand.begin(), seq_rand.end(), rng);

    cout << "1: Wstawianie rosnace:" << endl;
    Node* root1 = nullptr;
    for (int val : seq_inc) {
        cout << "\ninsert " << val << endl;
        root1 = insert(root1, val);
        show_tree(root1);
    }

    cout << "\n 1: Usuwanie:" << endl;
    vector<int> del_rand1 = seq_inc;
    shuffle(del_rand1.begin(), del_rand1.end(), rng);
    for (int val : del_rand1) {
        cout << "\ndelete " << val << endl;
        root1 = deleteNode(root1, val);
        show_tree(root1);
    }

    cout << "\n 2:Wstawianie losowe" << endl;
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