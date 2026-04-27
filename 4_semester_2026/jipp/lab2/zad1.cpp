#include<exception>
#include<iostream>
using namespace std;
template<unsigned int n> class Ring{
    private:
    unsigned int a;
    public:
    Ring(){
        a=0;
    }
    Ring(unsigned int x) {
        a = x % n;
    }
    Ring<n> operator+(const Ring<n>& r) const {
        unsigned int b = (a + r.a) % n;
        Ring<n> result(b);
        return result;
    }
    Ring<n> operator-(const Ring<n>& r) const {
        unsigned int b = (n + a - r.a) % n;
        Ring<n> result(b);
        return result;
    }    
    Ring<n> operator*(const Ring<n>& r) const {
        unsigned int b = (a * r.a) % n;
        Ring<n> result(b);
        return result;
    }
    Ring<n> operator/(const Ring<n>& r) const {
        for(int i=1; i<n; i++){
            if((r.a * i) % n == 1){
                unsigned int b = (a * i) % n;
                Ring<n> result(b);
                return result;
            }
        }
        throw invalid_argument("Brak rozwiazan");
    }
    bool operator==(const Ring<n>& r) const {
        return a == r.a;
    }
    bool operator!=(const Ring<n>& r) const {
        return a != r.a;
    }
    bool operator<=(const Ring<n>& r) const {
        return a <= r.a;
    }
    bool operator>=(const Ring<n>& r) const {
        return a >= r.a;
    }
    bool operator<(const Ring<n>& r) const {
        return a < r.a;
    }
    bool operator>(const Ring<n>& r) const {
        return a > r.a;
    }
    Ring<n>& operator+=(const Ring<n>& r) {
        a = (a + r.a) % n;
        return *this;
    }
    Ring<n>& operator-=(const Ring<n>& r) {
        a = (n + a - r.a) % n;
        return *this;
    }
    Ring<n>& operator=(const Ring<n>& r) {
        a = r.a;
        return *this;
    }
    Ring<n>& operator*=(const Ring<n>& r) {
        a = (a * r.a) % n;
        return *this;
    }
    Ring<n>& operator/=(const Ring<n>& r) {
        for(int i=1; i<n; i++){
            if((r.a * i) % n == 1){
                a = (a * i) % n;
                return *this;
            }
        }
        throw invalid_argument("Brak rozwiazan");
    }
    friend ostream& operator<<(ostream& os, const Ring<n>& r) {
        os << r.a;
        return os;
    }
    friend istream& operator>>(istream& is, Ring<n>& r) {
        unsigned int temp;
        is >> temp;
        r.a = temp % n;
        return is;
    }
};   
int main() {
    try {
        Ring<7> a(10); 
        Ring<7> b(5);  
        Ring<7> c;    

        cout << "a: " << a << endl;
        cout << "b: " << b << endl;
        cout << "a + b = " << a + b << endl;
        cout << "a - b = " << a - b << endl;
        cout << "a * b = " << a * b << endl;
        cout << "a / b = " << a / b << endl;

        cout << boolalpha; 
        cout << "a == b: " << (a == b) << endl;
        cout << "a != b: " << (a != b) << endl;
        cout << "a < b:  " << (a < b) << endl;
        cout << "a > b:  " << (a > b) << endl;
        cout << "a <= 3: " << (a <= Ring<7>(3)) << endl;

        c = a;
        cout << "c = a:     " << c << endl;
        c += b;
        cout << "c += b:    " << c  << endl;
        c -= b;
        cout << "c -= b:    " << c  << endl;
        c *= b;
        cout << "c *= b:    " << c << endl;
        c /= b;
        cout << "c /= b:    " << c << endl;

        cout<<"Podaj liczbe: "<<endl;
        cin >> c;
        cout << "Wprowadzono " << c << endl;

        Ring<7> zero(0);
        cout << "Proba dzielenia przez zero: ";
        cout << a / zero << endl;

    } catch (const exception& e) {
        cerr << "wyjatek:" << e.what() << endl;
    }

    return 0;
}