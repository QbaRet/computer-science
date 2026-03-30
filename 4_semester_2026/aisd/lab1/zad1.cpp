#include<bits/stdc++.h>
using namespace std;
class Queue{
    struct Node{
        int val;
        Node* next;
        void print(){
            cout<<val<<" ";
        }
    };
    Node* head;
public:
    Queue(){
        head=nullptr;
    }
    ~Queue(){
        while(head!=nullptr){
            Node* temp=head;
            head=head->next;
            delete temp;
        }
    }
    void push(int val){
        Node* newNode=new Node{val,nullptr};
        if(head==nullptr){
            head=newNode;
        }else{
            Node* temp=head;
            while(temp->next!=nullptr){
                temp=temp->next;
            }
            temp->next=newNode;
        }
    }
    bool pop(int& outVal){
        if(head==nullptr) return false;
        Node* temp=head;
        outVal=temp->val;
        head=head->next;
        delete temp;
        return true;
    }
    void print(){
        Node* temp=head;
        while(temp!=nullptr){
            temp->print();
            temp=temp->next;
        }
        cout<<endl;
    }
};
class Stack{
    struct Node{
        int val;
        Node* next;
        void print(){
            cout<<val<<" ";
        }
    };
    Node* top;
public:
    Stack(){
        top=nullptr;
    }
    ~Stack(){
        while(top!=nullptr){
            Node* temp=top;
            top=top->next;
            delete temp;
        }
    }
    void push(int val){
        Node* newNode=new Node{val,top};
        top=newNode;
    }
    bool pop(int& outVal){
        if(top==nullptr) return false;
        Node* temp=top;
        outVal=temp->val;
        top=top->next;
        delete temp;
        return true;
    }
    void print(){
        Node* temp=top;
        while(temp!=nullptr){
            temp->print();
            temp=temp->next;
        }
        cout<<endl;
    }
};

int main(){
    Queue q;
    Stack s;

    cout<<"QUEUE\n";
    for(int i=0;i<50;i++){
        q.push(i);
        cout<<"Pushed: "<<i<<"\n";
    }

    for(int i=0;i<51;i++){
        int qVal;
        if(q.pop(qVal)){
            cout<<"Popped: "<<qVal<<"\n";
        } else {
            cout<<"Queue is empty\n";
        }
    }
    cout<<"\nSTACK\n";
    for(int i=0;i<50;i++){
        s.push(i);
        cout<<"Pushed: "<<i<<"\n";
    }

    for(int i=0;i<51;i++){
        int sVal;
        if(s.pop(sVal)){
            cout<<"Popped: "<<sVal<<"\n";
        } else {
            cout<<"Stack is empty\n";
        }
    }

    return 0;
}