#include <exception>
#include <iostream>
#include <numeric>
#include <cstdlib> 
#include <ctime>

using namespace std;

template<unsigned int n> class Ring {
private:
    unsigned int a;
public:
    Ring() {
        a = 0;
    }
    Ring(unsigned int x) {
        a = x % n;
    }
    Ring<n> operator+(const Ring<n>& r) const {
        unsigned int b = (a + r.a) % n;
        return Ring<n>(b);
    }
    Ring<n> operator-(const Ring<n>& r) const {
        unsigned int b = (n + a - r.a) % n;
        return Ring<n>(b);
    }    
    Ring<n> operator*(const Ring<n>& r) const {
        unsigned int b = (1ULL * a * r.a) % n; 
        return Ring<n>(b);
    }
    Ring<n> operator/(const Ring<n>& r) const {
        for(unsigned int i = 1; i < n; i++){
            if((1ULL * r.a * i) % n == 1){
                unsigned int b = (1ULL * a * i) % n;
                return Ring<n>(b);
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
        a = (1ULL * a * r.a) % n;
        return *this;
    }
    Ring<n>& operator/=(const Ring<n>& r) {
        for(unsigned int i = 1; i < n; i++){
            if((1ULL * r.a * i) % n == 1){
                a = (1ULL * a * i) % n;
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


unsigned int nwd(unsigned int a, unsigned int b) {
    while (b != 0) {
        unsigned int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

template <typename T> class RSA {
private:
    unsigned int n;
    unsigned int fi;
    unsigned int e; 
    unsigned int d; 

public:
    RSA() {
        n = 0;
        fi = 0;
        e = 0;
        d = 0;
    }
    
    RSA(unsigned int p, unsigned int q) {
        n = p * q;
        fi = (p - 1) * (q - 1);
        while(true){
            e = rand() % (fi - 1) + 1;
            if(nwd(e, fi) == 1) break;
        }     
        
        long long t = 0, newt = 1;
        long long r = fi, newr = e;
        
        while (newr != 0) {
            long long quotient = r / newr;
            
            long long temp_t = t - quotient * newt;
            t = newt;
            newt = temp_t;
            
            long long temp_r = r - quotient * newr;
            r = newr;
            newr = temp_r;
        }
        
        if (t < 0) {
            t = t + fi;
        }
        d = static_cast<unsigned int>(t);
    }

    unsigned int getModulo() const {
        return n;
    }
   
    T getPublicKey() const {
        return T(e); 
    }

    T encrypt(T m) const {
        T result(1); 
        T base = m; 
        unsigned int exp = e;
        

        while (exp > 0) {
            if (exp % 2 == 1) {
                result = result * base; 
            }
            base = base * base;     
            exp /= 2;
        }
        return result;
    }
  
    T decrypt(T c) const {
        T result(1); 
        T base = c;  
        unsigned int exp = d;

        while (exp > 0) {
            if (exp % 2 == 1) {
                result = result * base; 
            }
            base = base * base;        
            exp /= 2;
        }
        return result;
    }
};

int main() {
    srand(time(NULL)); 

    try {
        constexpr unsigned int p = 10007;
        constexpr unsigned int q = 10009;
        constexpr unsigned int N = p * q; 
        
        RSA< Ring<N> > rsa(p, q);
        
        cout << "--- Parametry RSA ---" << endl;
        cout << "Modulo (n): " << rsa.getModulo() << endl;
        cout << "Klucz publiczny (e): " << rsa.getPublicKey() << endl;

        Ring<N> m(1234567); 
        cout << "\n--- Szyfrowanie ---" << endl;
        cout << "Oryginalna wiadomosc (m): " << m << endl;
        
        Ring<N> c = rsa.encrypt(m);
        cout << "Szyfrogram (c): " << c << endl;
        
        Ring<N> zdekodowana = rsa.decrypt(c);
        cout << "Zdekodowana wiadomosc: " << zdekodowana << endl;


    } catch (const exception& e) {
        cerr << "Wyjatek: " << e.what() << endl;
    }

    return 0;
}