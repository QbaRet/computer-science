#include <iostream>
#include <vector>
#include <cmath>
#include <tuple>
#include <stdexcept>

using namespace std;

// Szablon klasy wielomianu nad ciałem K
template <typename K>
class Poly {
public:
    vector<K> coef;

    void clean() {
        // ZMIANA: Zwiększona tolerancja na 1e-5 dla ułamków niewymiernych
        while (!coef.empty() && abs(coef.back()) < 1e-5) {
            coef.pop_back();
        }
    }

    Poly() {}

    Poly(vector<K> c) : coef(c) {
        clean();
    }

    int degree() const {
        return coef.empty() ? -1 : coef.size() - 1;
    }

    K leading_coeff() const {
        return coef.empty() ? 0 : coef.back();
    }

    bool is_zero() const {
        return coef.empty();
    }

    Poly<K> make_monic() const {
        if (is_zero()) return *this;
        K lc = leading_coeff();
        Poly<K> res = *this;
        for (auto& c : res.coef) c /= lc;
        return res;
    }

    Poly<K> operator+(const Poly<K>& other) const {
        Poly<K> res;
        size_t maxSize = max(coef.size(), other.coef.size());
        res.coef.resize(maxSize, 0);
        for (size_t i = 0; i < maxSize; ++i) {
            if (i < coef.size()) res.coef[i] += coef[i];
            if (i < other.coef.size()) res.coef[i] += other.coef[i];
        }
        res.clean();
        return res;
    }

    Poly<K> operator-(const Poly<K>& other) const {
        Poly<K> res;
        size_t maxSize = max(coef.size(), other.coef.size());
        res.coef.resize(maxSize, 0);
        for (size_t i = 0; i < maxSize; ++i) {
            if (i < coef.size()) res.coef[i] += coef[i];
            if (i < other.coef.size()) res.coef[i] -= other.coef[i];
        }
        res.clean();
        return res;
    }

    Poly<K> operator*(const Poly<K>& other) const {
        if (is_zero() || other.is_zero()) return Poly<K>();
        Poly<K> res;
        res.coef.resize(coef.size() + other.coef.size() - 1, 0);
        for (size_t i = 0; i < coef.size(); ++i) {
            for (size_t j = 0; j < other.coef.size(); ++j) {
                res.coef[i + j] += coef[i] * other.coef[j];
            }
        }
        res.clean();
        return res;
    }

    Poly<K> operator*(K scalar) const {
        Poly<K> res = *this;
        for (auto& c : res.coef) c *= scalar;
        res.clean();
        return res;
    }

    Poly<K> divmod(Poly<K> B, Poly<K>& R) const {
        if (B.is_zero()) throw invalid_argument("Dzielenie przez zero!");
        Poly<K> Q;
        R = *this;

        while (!R.is_zero() && R.degree() >= B.degree()) {
            int deg_diff = R.degree() - B.degree();
            K ratio = R.leading_coeff() / B.leading_coeff();
            
            vector<K> term_coef(deg_diff + 1, 0);
            term_coef.back() = ratio;
            Poly<K> term(term_coef);
            
            Q = Q + term;
            R = R - (B * term);
        }
        return Q;
    }

    Poly<K> operator/(const Poly<K>& B) const { Poly<K> R; return divmod(B, R); }
    Poly<K> operator%(const Poly<K>& B) const { Poly<K> R; divmod(B, R); return R; }

    friend ostream& operator<<(ostream& os, const Poly<K>& p) {
        if (p.is_zero()) return os << "0";
        bool first = true;
        for (int i = p.degree(); i >= 0; --i) {
            K c = p.coef[i];
            // ZMIANA: Zwiększona tolerancja przy wyświetlaniu
            if (abs(c) < 1e-5) continue;

            if (c > 0 && !first) os << " + ";
            if (c < 0) {
                if (first) os << "-"; else os << " - ";
            }
            
            K val = abs(c);
            if (val != 1.0 || i == 0) os << val;
            if (i == 1) os << "x";
            else if (i > 1) os << "x^" << i;
            
            first = false;
        }
        return os;
    }
};

template <typename K>
Poly<K> NWD(Poly<K> A, Poly<K> B) {
    while (!B.is_zero()) { Poly<K> tmp = B; B = A % B; A = tmp; }
    return A.make_monic();
}

template <typename K>
Poly<K> NWW(Poly<K> A, Poly<K> B) {
    if (A.is_zero() || B.is_zero()) return Poly<K>();
    return ((A * B) / NWD(A, B)).make_monic();
}

template <typename K>
tuple<Poly<K>, Poly<K>, Poly<K>> ext_NWD(Poly<K> A, Poly<K> B) {
    Poly<K> old_r = A, r = B;
    Poly<K> old_s({1.0}), s;
    Poly<K> old_t, t({1.0});

    while (!r.is_zero()) {
        Poly<K> q = old_r / r;
        Poly<K> tmp_r = r; r = old_r - (q * r); old_r = tmp_r;
        Poly<K> tmp_s = s; s = old_s - (q * s); old_s = tmp_s;
        Poly<K> tmp_t = t; t = old_t - (q * t); old_t = tmp_t;
    }
    K lc = old_r.leading_coeff();
    return make_tuple(old_r.make_monic(), old_s * (1.0/lc), old_t * (1.0/lc));
}

int main() {
    typedef Poly<double> PolyR;

    cout << "Zmienne: a=2, b=8, c=3, d=9, e=8, f=2\n\n";

    cout << "a)\n";
    // cx^a + b -> 3x^2 + 8
    PolyR P1({8.0, 0.0, 3.0});
    // x + 1
    PolyR P2({1.0, 1.0});

    cout << "N(" << P1 << ") = " << P1.degree() << "\n";
    
    PolyR R;
    PolyR Q = P1.divmod(P2, R);
    cout << "(" << P1 << ") / (" << P2 << ") = " << Q << ", reszta: " << R << "\n\n";

    cout << "b)\n";
    // v(x) = ax^3 + bx^2 + cx + d -> 2x^3 + 8x^2 + 3x + 9
    PolyR V({9.0, 3.0, 8.0, 2.0});
    // w(x) = dx^3 + ex^2 + fx     -> 9x^3 + 8x^2 + 2x
    PolyR W({0.0, 2.0, 8.0, 9.0});

    cout << "v(x) = " << V << "\n";
    cout << "w(x) = " << W << "\n";

    PolyR gcd_vw, s, t;
    tie(gcd_vw, s, t) = ext_NWD(V, W);
    cout << "ext_NWD(v, w) = " << gcd_vw << "\n\n";

    // Wyliczanie g
    double x_root = -4.0; 
    for(int i = 0; i < 50; ++i) {
        double v_val = 2*pow(x_root, 3) + 8*pow(x_root, 2) + 3*x_root + 9;
        double v_der = 6*pow(x_root, 2) + 16*x_root + 3;
        x_root -= v_val / v_der;
    }
    double g_val = -(9*pow(x_root, 3) + 8*pow(x_root, 2) + 2*x_root);
    
    PolyR W_g = W + PolyR({g_val}); 

    cout << "g = " << g_val << "\n";
    cout << "w(x)+g = " << W_g << "\n";
    cout << "NWD(v, w+g) = " << NWD(V, W_g) << "\n";
    cout << "NWW(v, w+g) = " << NWW(V, W_g) << "\n";

    return 0;
}