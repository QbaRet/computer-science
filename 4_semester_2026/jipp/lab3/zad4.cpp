#include <iostream>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

using namespace std;

template<unsigned int n> class Ring {
private:
    unsigned int a;

    unsigned int modInverse(unsigned int val) const {
        long long t = 0, newt = 1;
        long long r = n, newr = val;
        while (newr != 0) {
            long long quotient = r / newr;
            long long temp_t = t - quotient * newt;
            t = newt;
            newt = temp_t;
            long long temp_r = r - quotient * newr;
            r = newr;
            newr = temp_r;
        }
        if (r > 1) throw invalid_argument("Brak odwrotnosci (dzielenie przez zero lub brak wzglednej pierwszosci)");
        if (t < 0) t = t + n;
        return static_cast<unsigned int>(t);
    }

public:
    Ring() : a(0) {}
    Ring(unsigned int x) : a(x % n) {}

    Ring<n> operator+(const Ring<n>& r) const { return Ring<n>((a + r.a) % n); }
    Ring<n> operator-(const Ring<n>& r) const { return Ring<n>((n + a - r.a) % n); }    
    Ring<n> operator*(const Ring<n>& r) const { return Ring<n>((1ULL * a * r.a) % n); }
    
    Ring<n> operator/(const Ring<n>& r) const {
        unsigned int inv = modInverse(r.a);
        return Ring<n>((1ULL * a * inv) % n);
    }

    bool operator==(const Ring<n>& r) const { return a == r.a; }
    bool operator!=(const Ring<n>& r) const { return a != r.a; }

    friend ostream& operator<<(ostream& os, const Ring<n>& r) {
        os << r.a;
        return os;
    }
};

template <typename T>
class DHSetup {
private:
    T generator;

public:
    DHSetup() {
        unsigned int r = rand() % 1000000000 + 2;
        generator = T(r);
    }

    T getGenerator() const {
        return generator;
    }

    T power(T base, unsigned long b) const {
        T result(1);
        while (b > 0) {
            if (b % 2 == 1) {
                result = result * base;
            }
            base = base * base;
            b /= 2;
        }
        return result;
    }
};

template <typename T>
class User {
private:
    DHSetup<T>& setup;
    unsigned long secret;
    T key;
    bool is_key_set;

public:
    User(DHSetup<T>& setup_ref) : setup(setup_ref), is_key_set(false) {
        secret = rand() % 1000000000 + 1; 
    }

    T getPublicKey() const {
        return setup.power(setup.getGenerator(), secret);
    }

    void setKey(T other_public_key) {
        key = setup.power(other_public_key, secret);
        is_key_set = true;
    }

    T encrypt(T m) const {
        if (!is_key_set) {
            throw logic_error("Blad: Proba szyfrowania przed ustawieniem klucza wspolnego!");
        }
        return m * key;
    }

    T decrypt(T c) const {
        if (!is_key_set) {
            throw logic_error("Blad: Proba deszyfrowania przed ustawieniem klucza wspolnego!");
        }
        return c / key;
    }
};

int main() {
    srand(static_cast<unsigned int>(time(NULL)));

    try {
        constexpr unsigned int p = 1234567891;
        using MyField = Ring<p>;

        cout << "--- Inicjalizacja Diffie-Hellman ---" << endl;
        DHSetup<MyField> dh;
        cout << "Wylosowany generator ciala: " << dh.getGenerator() << "\n\n";

        User<MyField> alice(dh);
        User<MyField> bob(dh);

        MyField pub_alice = alice.getPublicKey();
        MyField pub_bob = bob.getPublicKey();

        cout << "Klucz publiczny Alice: " << pub_alice << endl;
        cout << "Klucz publiczny Boba:  " << pub_bob << "\n\n";

        try {
            cout << "Proba szyfrowania bez klucza..." << endl;
            alice.encrypt(MyField(123));
        } catch (const exception& e) {
            cout << "Zlapano wyjatek: " << e.what() << "\n\n";
        }

        alice.setKey(pub_bob);
        bob.setKey(pub_alice);

        MyField oryginalna_wiadomosc(987654321);
        cout << "--- Komunikacja (Alice -> Bob) ---" << endl;
        cout << "Wiadomosc Alice: " << oryginalna_wiadomosc << endl;

        MyField szyfrogram = alice.encrypt(oryginalna_wiadomosc);
        cout << "Zaszyfrowana wiadomosc (wyslana w swiat): " << szyfrogram << endl;

        MyField odebrana_wiadomosc = bob.decrypt(szyfrogram);
        cout << "Wiadomosc odkodowana przez Boba: " << odebrana_wiadomosc << endl;

        if (oryginalna_wiadomosc == odebrana_wiadomosc) {
            cout << "\nSukces! Wymiana i deszyfrowanie dzialaja bezblednie." << endl;
        }

    } catch (const exception& e) {
        cerr << "Krytyczny wyjatek: " << e.what() << endl;
    }

    return 0;
}