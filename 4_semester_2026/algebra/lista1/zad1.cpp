#include <iostream>
#include <vector>
#include <cmath>
#include <exception>

using namespace std;

class GaussInt {
private:
    long long a, b;

public:
    GaussInt(long long real = 0, long long imag = 0) : a(real), b(imag) {}

    long long norm() const {
        return a * a + b * b;
    }

    GaussInt operator+(const GaussInt& r) const { return GaussInt(a + r.a, b + r.b); }
    GaussInt operator-(const GaussInt& r) const { return GaussInt(a - r.a, b - r.b); }
    GaussInt operator*(const GaussInt& r) const { return GaussInt(a * r.a - b * r.b, a * r.b + b * r.a); }

    GaussInt operator/(const GaussInt& r) const {
        if (r.a == 0 && r.b == 0) throw invalid_argument("Dzielenie przez zero");
        long long mianownik = r.norm();
        double r_part = static_cast<double>(a * r.a + b * r.b) / mianownik;
        double i_part = static_cast<double>(b * r.a - a * r.b) / mianownik;
        return GaussInt(round(r_part), round(i_part));
    }

    GaussInt operator%(const GaussInt& r) const {
        return (*this) - ((*this / r) * r);
    }

    bool operator==(const GaussInt& r) const { return a == r.a && b == r.b; }
    bool operator!=(const GaussInt& r) const { return !(*this == r); }

    friend ostream& operator<<(ostream& os, const GaussInt& g) {
        if (g.a == 0 && g.b == 0) return os << "0";
        if (g.a != 0) os << g.a;
        if (g.b > 0) os << (g.a != 0 ? "+" : "") << (g.b == 1 ? "" : to_string(g.b)) << "i";
        else if (g.b < 0) os << (g.b == -1 ? "-" : to_string(g.b)) << "i";
        return os;
    }
};

GaussInt NWD(GaussInt x, GaussInt y) {
    while (y != GaussInt(0, 0)) {
        GaussInt reszta = x % y;
        x = y;
        y = reszta;
    }
    return x;
}

GaussInt NWW(GaussInt x, GaussInt y) {
    if (x == GaussInt(0,0) || y == GaussInt(0,0)) return GaussInt(0,0);
    return (x * y) / NWD(x, y); 
}

GaussInt NWD_lista(const vector<GaussInt>& lista) {
    if (lista.empty()) throw invalid_argument("Brak mozliwosci obliczenia NWD");
    if (lista.size() == 1) return lista[0];
    GaussInt wynik = lista[0];
    for (size_t i = 1; i < lista.size(); i++) wynik = NWD(wynik, lista[i]);
    return wynik;
}

GaussInt NWW_lista(const vector<GaussInt>& lista) {
    if (lista.empty()) throw invalid_argument("Brak mozliwosci obliczenia NWW");
    if (lista.size() == 1) return lista[0];
    GaussInt wynik = lista[0];
    for (size_t i = 1; i < lista.size(); i++) wynik = NWW(wynik, lista[i]);
    return wynik;
}

int main() {
    long long a = 2, b = 8, c = 3, d = 9, e = 8, f = 2;
    cout << "Zmienne: a="<<a<<", b="<<b<<", c="<<c<<", d="<<d<<", e="<<e<<", f="<<f<<"\n\n";

    cout << "b)\n";
    GaussInt n_val(a, b);
    cout << "N(" << n_val << ") = " << n_val.norm() << "\n";

    GaussInt dzielna(c + a, d + b);
    GaussInt dzielnik(e, f);
    cout << "(" << dzielna << ") / (" << dzielnik << ") = " 
         << dzielna / dzielnik << ", reszta: " << dzielna % dzielnik << "\n\n";

    cout << "c)\n";
    GaussInt t1(a, b), t2(c, d), t3(e, d);
    vector<GaussInt> trojka = {t1, t2, t3};
    
    cout << "Trojka: " << t1 << ", " << t2 << ", " << t3 << "\n";
    cout << "NWD: " << NWD_lista(trojka) << " | NWW: " << NWW_lista(trojka) << "\n\n";

    cout << "Lista 1-elementowa [" << t1 << "]:\n";
    cout << "NWD: " << NWD_lista({t1}) << " | NWW: " << NWW_lista({t1}) << "\n\n";

    cout << "Lista pusta []:\n";
    try {
        cout << NWD_lista({}) << "\n";
    } catch (const exception& ex) {
        cout << ex.what() << "\n";
    }

    return 0;
}