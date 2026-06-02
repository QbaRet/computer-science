#include <bits/stdc++.h>
using namespace std;

// ============================================================
// a) Struktura wielomianu wielu zmiennych
// ============================================================

// Monomial: wektor wykładników dla n zmiennych + współczynnik
struct Monomial {
    vector<int> exp;   // exp[i] = wykładnik i-tej zmiennej
    double coef;       // współczynnik rzeczywisty

    int numVars() const { return (int)exp.size(); }

    // Stopień łączny
    int totalDeg() const {
        return accumulate(exp.begin(), exp.end(), 0);
    }

    // Czy jednomian jest zerem?
    bool isZero() const { return abs(coef) < 1e-12; }

    // Iloczyn jednomianów (mnożenie)
    Monomial operator*(const Monomial& o) const {
        Monomial res;
        res.coef = coef * o.coef;
        res.exp.resize(exp.size());
        for (int i = 0; i < (int)exp.size(); i++)
            res.exp[i] = exp[i] + o.exp[i];
        return res;
    }

    // Dzielenie jednomianów (zakłada podzielność)
    Monomial operator/(const Monomial& o) const {
        Monomial res;
        res.coef = coef / o.coef;
        res.exp.resize(exp.size());
        for (int i = 0; i < (int)exp.size(); i++)
            res.exp[i] = exp[i] - o.exp[i];
        return res;
    }

    // Czy this jest podzielne przez o (NWW wykładników)?
    bool divisibleBy(const Monomial& o) const {
        for (int i = 0; i < (int)exp.size(); i++)
            if (exp[i] < o.exp[i]) return false;
        return true;
    }

    string toString(const vector<string>& vars) const {
        if (isZero()) return "0";
        ostringstream oss;
        oss << fixed << setprecision(4);
        if (abs(coef - 1.0) > 1e-12 || totalDeg() == 0) oss << coef;
        for (int i = 0; i < (int)vars.size(); i++) {
            if (exp[i] == 1) oss << vars[i];
            else if (exp[i] > 1) oss << vars[i] << "^" << exp[i];
        }
        return oss.str();
    }
};

// ============================================================
// b) Porządki na jednomianach
// ============================================================

// Typ porządku
enum MonomialOrder { LEX, PERM_LEX, GRADED_LEX };

// Komparator porządku leksykograficznego (standardowy)
// x1 > x2 > ... > xn
bool lexLess(const Monomial& a, const Monomial& b) {
    for (int i = 0; i < (int)a.exp.size(); i++) {
        if (a.exp[i] != b.exp[i]) return a.exp[i] < b.exp[i];
    }
    return false; // równe
}

// Permutowany Lex: porównuje według permutacji zmiennych
// perm[i] = indeks i-tej zmiennej w porządku (malejąco ważnej)
bool permLexLess(const Monomial& a, const Monomial& b,
                 const vector<int>& perm) {
    for (int idx : perm) {
        if (a.exp[idx] != b.exp[idx])
            return a.exp[idx] < b.exp[idx];
    }
    return false;
}

// GradedLex: najpierw stopień łączny, potem Lex
bool gradedLexLess(const Monomial& a, const Monomial& b) {
    int da = a.totalDeg(), db = b.totalDeg();
    if (da != db) return da < db;
    return lexLess(a, b);
}

// Ogólny komparator (zwraca true jeśli a < b w danym porządku)
bool monomialLess(const Monomial& a, const Monomial& b,
                  MonomialOrder ord, const vector<int>& perm) {
    switch (ord) {
        case LEX:        return lexLess(a, b);
        case PERM_LEX:   return permLexLess(a, b, perm);
        case GRADED_LEX: return gradedLexLess(a, b);
    }
    return false;
}

// ============================================================
// Wielomian = lista jednomianów (posortowana malejąco wg porządku)
// ============================================================

struct Polynomial {
    vector<Monomial> terms;  // posortowane malejąco (największy = terms[0])
    int n;                   // liczba zmiennych
    vector<string> vars;     // nazwy zmiennych

    Polynomial(int numVars, vector<string> varNames)
        : n(numVars), vars(varNames) {}

    // Dodanie jednomianu (bez sortowania – call normalize() potem)
    void addMonomial(Monomial m) {
        if (!m.isZero()) terms.push_back(m);
    }

    // Normalizacja: sumowanie like-terms i sortowanie
    void normalize(MonomialOrder ord, const vector<int>& perm) {
        // Zgrupuj wykładniki
        map<vector<int>, double> acc;
        for (auto& m : terms) acc[m.exp] += m.coef;
        terms.clear();
        for (auto& [e, c] : acc) {
            if (abs(c) > 1e-12) {
                Monomial m; m.exp = e; m.coef = c;
                terms.push_back(m);
            }
        }
        // Sortuj malejąco
        sort(terms.begin(), terms.end(),
             [&](const Monomial& a, const Monomial& b) {
                 return monomialLess(b, a, ord, perm); // odwrócone -> malejąco
             });
    }

    bool isZero() const { return terms.empty(); }

    // Wiodący jednomian (leading term)
    const Monomial& LT() const { return terms[0]; }

    // Wiodący współczynnik
    double LC() const { return terms[0].coef; }

    // Wiodący wykładnik
    const vector<int>& LM() const { return terms[0].exp; }

    string toString() const {
        if (terms.empty()) return "0";
        string s;
        for (int i = 0; i < (int)terms.size(); i++) {
            if (i > 0 && terms[i].coef > 0) s += " + ";
            else if (i > 0) s += " ";
            s += terms[i].toString(vars);
        }
        return s;
    }

    // Suma wielomianów
    Polynomial operator+(const Polynomial& o) const {
        Polynomial res(n, vars);
        res.terms = terms;
        for (auto& m : o.terms) res.terms.push_back(m);
        return res;
    }

    // Różnica
    Polynomial operator-(const Polynomial& o) const {
        Polynomial res(n, vars);
        res.terms = terms;
        for (auto m : o.terms) { m.coef = -m.coef; res.terms.push_back(m); }
        return res;
    }

    // Mnożenie przez jednomian
    Polynomial mulMonomial(const Monomial& m) const {
        Polynomial res(n, vars);
        for (auto& t : terms) res.terms.push_back(t * m);
        return res;
    }
};

// ============================================================
// c) PolynomialReduce
// Wejście: f, ciąg G = (g1,...,gk), porządek
// Wyjście: alpha[0..k-1], reszta r
// Niezmiennik: f = sum(alpha_i * g_i) + r
// ============================================================

struct ReduceResult {
    vector<Polynomial> alpha;  // współczynniki
    Polynomial r;              // reszta
};

ReduceResult PolynomialReduce(
        Polynomial f,
        const vector<Polynomial>& G,
        MonomialOrder ord,
        const vector<int>& perm)
{
    int k = (int)G.size();
    int nv = f.n;

    // Inicjalizacja: alpha_i = 0-wielomian, r = 0
    ReduceResult res { vector<Polynomial>(k, Polynomial(nv, f.vars)),
                       Polynomial(nv, f.vars) };

    auto zero = [&]() { return Polynomial(nv, f.vars); };

    f.normalize(ord, perm);

    while (!f.isZero()) {
        bool divOccurred = false;

        for (int i = 0; i < k && !f.isZero(); i++) {
            Polynomial& gi = const_cast<Polynomial&>(G[i]);
            gi.normalize(ord, perm);

            // Sprawdź czy LT(gi) | LT(f)
            while (!f.isZero() && f.LT().divisibleBy(gi.LT())) {
                // q = LT(f) / LT(gi)
                Monomial q = f.LT() / gi.LT();

                // alpha_i += q (jako wielomian jednowyrazowy)
                Polynomial qPoly(nv, f.vars);
                qPoly.addMonomial(q);
                qPoly.normalize(ord, perm);

                res.alpha[i] = (res.alpha[i] + qPoly);
                res.alpha[i].normalize(ord, perm);

                // f -= q * gi
                f = f - gi.mulMonomial(q);
                f.normalize(ord, perm);

                divOccurred = true;
            }
        }

        if (!divOccurred) {
            // LT(f) nie jest podzielny przez żaden LT(gi)
            // Przenieś LT(f) do reszty
            if (!f.isZero()) {
                Polynomial ltPoly(nv, f.vars);
                ltPoly.addMonomial(f.LT());
                res.r = res.r + ltPoly;
                res.r.normalize(ord, perm);

                // Odejmij LT(f) od f
                Polynomial ltNeg(nv, f.vars);
                Monomial neg = f.LT(); neg.coef = -neg.coef;
                ltNeg.addMonomial(neg);
                f = f + ltNeg;
                f.normalize(ord, perm);
            }
        }
    }

    return res;
}

// ============================================================
// Pomocnicze: drukowanie wyniku redukcji
// ============================================================

void printResult(const string& fname,
                 const vector<string>& gnames,
                 const ReduceResult& res,
                 const vector<string>& vars)
{
    cout << fname << " = ";
    for (int i = 0; i < (int)res.alpha.size(); i++) {
        string a = res.alpha[i].toString();
        if (a != "0") {
            cout << "(" << a << ") * " << gnames[i];
            cout << " + ";
        }
    }
    cout << "r,  gdzie r = " << res.r.toString() << "\n";
}

// ============================================================
// Pomocnicze: konstruktor wielomianu z listy (coef, exp...)
// ============================================================

Polynomial makePoly(int n, const vector<string>& vars,
                    const vector<pair<double,vector<int>>>& data)
{
    Polynomial p(n, vars);
    for (auto& [c,e] : data) {
        Monomial m; m.coef = c; m.exp = e;
        p.addMonomial(m);
    }
    return p;
}

// ============================================================
// MAIN – demonstracja (odpowiada zadaniu Lab 4)
// ============================================================

int main()
{
    cout << fixed << setprecision(4);

    // ---------------------------------------------------------
    // Przykład z Ćwiczenia 37 (część d)
    // Zmienne: x, y  (n=2)
    // Porządek domyślny: Lex  x > y
    // f = x^2*y + x*y^2 + y^2
    // G = (g1, g2) = (x*y - 1,  y^2 - 1)
    // ---------------------------------------------------------
    int n = 2;
    vector<string> vars = {"x","y"};
    vector<int> permStd = {0,1}; // standardowy Lex: x > y

    // f = x^2*y + x*y^2 + y^2
    Polynomial f = makePoly(n, vars, {
        {1.0, {2,1}},
        {1.0, {1,2}},
        {1.0, {0,2}}
    });

    // g1 = x*y - 1
    Polynomial g1 = makePoly(n, vars, {
        {1.0, {1,1}},
        {-1.0, {0,0}}
    });

    // g2 = y^2 - 1
    Polynomial g2 = makePoly(n, vars, {
        {1.0, {0,2}},
        {-1.0, {0,0}}
    });

    cout << "=== Lex, kolejnosc (g1, g2) ===\n";
    auto res1 = PolynomialReduce(f, {g1, g2}, LEX, permStd);
    printResult("f", {"g1","g2"}, res1, vars);

    cout << "\n=== Lex, kolejnosc (g2, g1) ===\n";
    auto res2 = PolynomialReduce(f, {g2, g1}, LEX, permStd);
    printResult("f", {"g2","g1"}, res2, vars);


    cout << "\n=== Czesc e): 3 rozne porzadki ===\n";
    int n3 = 3;
    vector<string> vars3 = {"x","y","z"};

    // h = x^2*y - y*z^2 + x*z^3
    Polynomial h = makePoly(n3, vars3, {
        {1.0, {2,1,0}},
        {-1.0,{0,1,2}},
        {1.0, {1,0,3}}
    });

    // Prosty ciąg G = (x - 1, y - 1, z - 1)
    Polynomial G1 = makePoly(n3, vars3, {{1.0,{1,0,0}},{-1.0,{0,0,0}}});
    Polynomial G2 = makePoly(n3, vars3, {{1.0,{0,1,0}},{-1.0,{0,0,0}}});
    Polynomial G3 = makePoly(n3, vars3, {{1.0,{0,0,1}},{-1.0,{0,0,0}}});
    vector<Polynomial> G3list = {G1,G2,G3};

    // 1) Lex: x > y > z
    vector<int> pLex  = {0,1,2};
    auto rLex  = PolynomialReduce(h, G3list, LEX, pLex);
    cout << "Lex (x>y>z):        r = " << rLex.r.toString() << "\n";

    // 2) GradedLex
    auto rGrLex = PolynomialReduce(h, G3list, GRADED_LEX, pLex);
    cout << "GradedLex:          r = " << rGrLex.r.toString() << "\n";

    // 3) Permutowany Lex: z > y > x  (perm = {2,1,0})
    vector<int> pPermZYX = {2,1,0};
    auto rPermLex = PolynomialReduce(h, G3list, PERM_LEX, pPermZYX);
    cout << "PermLex (z>y>x):    r = " << rPermLex.r.toString() << "\n";
>>>>>>> 9d6a37bdcc40a0a3bcf00fa80ee764d370e527e2

    return 0;
}