#include<bits/stdc++.h>
using namespace std;
class List{
    struct Node{
        int val;
        Node* next;
    };
    Node* cur;
    int size;
public:
    List(){
        cur = nullptr;
        size= 0;
    }
    ~List(){
       if(cur != nullptr){
            Node* temp = cur;
            while(temp->next != cur){
                temp = temp->next;
            }
            temp->next = nullptr; 
            while(cur != nullptr){
                Node* toDelete = cur;
                cur = cur->next;
                delete toDelete;
            }
        }
    }
    void insert(int val){
        Node* newNode=new Node;
        newNode->val = val;
        if(cur == nullptr){
            newNode->next = newNode;
            cur = newNode;
        } else {
            newNode->next = cur->next;
            cur->next = newNode;
        }
        size++;
    }
    int findCost(int target){
        if(cur==nullptr) return 0;
        int cost = 0;
        Node* temp = cur->next;
        for(int i = 0; i < size; i++){
            cost++;
            if(temp->val == target){
                return cost;
            };
            temp = temp->next;
        }
        return cost;
    }
    void merge(List& other){
        if(other.cur == nullptr) return;
        if(this->size==0){
            this->cur = other.cur;
            this->size = other.size;
        } else {
            Node* temp1 = this->cur->next;
            Node* temp2 = other.cur->next;
            this->cur->next = temp2;
            other.cur->next = temp1;
            this->cur = other.cur;
            this->size += other.size;
        }
        other.cur = nullptr;
        other.size = 0;
    }
    void display()
    {
        if(cur == nullptr) return;
        Node* temp = cur->next;
        for(int i = 0; i < size; i++){
            cout << temp->val << " ";
            temp = temp->next;
        }
        cout << endl;
    }
};
int main() {
    srand(time(NULL));
    List l1, l2;
    for (int i = 0; i < 10; i++) {
        l1.insert(10 + rand() % 90); 
        l2.insert(10 + rand() % 90);
    }
    cout << "Lista 1: "; l1.display();
    cout << "Lista 2: "; l2.display();
    
    l1.merge(l2);
    cout << "Po polaczeniu: "; l1.display();
    cout<<"_____________________________________________"<<endl;
    const int N = 10000;
    const int RANGE = 100001; 
    List L;
    vector<int> T;
    for (int i = 0; i < N; i++) {
        int val = rand() % RANGE;
        T.push_back(val);
        L.insert(val);
    }

    long long totalCostA = 0;
    for (int i = 0; i < 1000; i++) {
        int target = T[rand() % N];
        totalCostA += L.findCost(target);
    }

    long long totalCostB = 0;
    for (int i = 0; i < 1000; i++) {
        int target = rand() % RANGE;
        totalCostB += L.findCost(target);
    }
    cout << "Sredni koszt liczby z listy: " << totalCostA / 1000.0 << " porownan" << endl;
    cout << "Sredni koszt liczby losowe z I: " << totalCostB / 1000.0 << " porownan" << endl;

    return 0;
}