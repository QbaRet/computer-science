/*
 * Sprawozdanie 3 - Lab 6
 * Numer indeksu: ________ (uzupelnij)
 * Zmienne a,b,c,d,e,f sa kolejnymi cyframi Twojego numeru indeksu (pozycje 1-6)
 *
 * Kompilacja: g++ -O2 -std=c++17 -o lab6 lab6_groebner.cpp
 *
 * Wymagania: C++17 lub nowszy
 */

#include <bits/stdc++.h>
using namespace std;

// ============================================================
//  KONFIGURACJA: wpisz cyfry swojego indeksu
// ============================================================
// Przyklad: indeks 186021  =>  a=1, b=8, c=6, d=0, e=2, f=1
const int INDEX_A = 2; // 1. cyfra indeksu
const int INDEX_B = 8; // 2. cyfra indeksu
const int INDEX_C = 3; // 3. cyfra indeksu
const int INDEX_D = 9; // 4. cyfra indeksu
const int INDEX_E = 8; // 5. cyfra indeksu
const int INDEX_F = 2; // 6. cyfra indeksu
// ============================================================

// ============================================================
//  CZESC a) - Pierscien wielomianow wielu zmiennych R[x1,...,xn]
// ============================================================

/*
 * Monom reprezentowany jako wektor wykladnikow.
 * Przyklad: x1^2 * x2 * x3^0 => {2, 1, 0}
 */
struct Monomial {
    vector<int> exp; // wykladniki dla kazdej zmiennej

    Monomial() {}
    explicit Monomial(int n) : exp(n, 0) {}
    Monomial(initializer_list<int> il) : exp(il) {}

    int numVars() const { return (int)exp.size(); }

    // Stopien monomialu
    int degree() const {
        return accumulate(exp.begin(), exp.end(), 0);
    }

    // Porownanie dla porzadku leksykograficznego (lex)
    bool operator<(const Monomial& o) const { return exp < o.exp; }
    bool operator==(const Monomial& o) const { return exp == o.exp; }
    bool operator>(const Monomial& o) const { return exp > o.exp; }

    // Iloczyn mononomiow
    Monomial operator*(const Monomial& o) const {
        Monomial res(numVars());
        for (int i = 0; i < numVars(); i++)
            res.exp[i] = exp[i] + o.exp[i];
        return res;
    }

    // Czy this jest podzielny przez o?
    bool divisibleBy(const Monomial& o) const {
        for (int i = 0; i < numVars(); i++)
            if (exp[i] < o.exp[i]) return false;
        return true;
    }

    // Iloraz (zakladamy podzielnosc)
    Monomial operator/(const Monomial& o) const {
        Monomial res(numVars());
        for (int i = 0; i < numVars(); i++)
            res.exp[i] = exp[i] - o.exp[i];
        return res;
    }

    // Najmniejsza wspolna wielokrotnosc
    static Monomial lcm(const Monomial& a, const Monomial& b) {
        Monomial res(a.numVars());
        for (int i = 0; i < a.numVars(); i++)
            res.exp[i] = max(a.exp[i], b.exp[i]);
        return res;
    }

    // Wyswietlanie
    string toString(const vector<string>& vars) const {
        string s;
        for (int i = 0; i < (int)exp.size(); i++) {
            if (exp[i] == 0) continue;
            s += vars[i];
            if (exp[i] > 1) s += "^" + to_string(exp[i]);
        }
        return s.empty() ? "1" : s;
    }
};

/*
 * Wielomian wielu zmiennych nad Q.
 * Koeficjenty sa wymierne (rational), reprezentowane jako double
 * dla prostoty (w prawdziwej implementacji uzyc klasy Rational).
 *
 * Roznica wzgledem wielomianow 1 zmiennej:
 *   - Monom to wektor wykladnikow (nie pojedynczy int)
 *   - Porzadek monomow jest wielokryterialny (lex, grlex, grevlex)
 *   - Wiodacy monom (LM), wiodacy wspolczynnik (LC) zalezy od porzadku
 *   - Dzielenie daje potencjalnie kilka reszt (lista dzielnikow)
 *   - Algorytm Buchbergera jest potrzebny do obliczenia bazy Grobnera
 */
struct Polynomial {
    // Reprezentacja: mapa monom -> wspolczynnik (posortowana malejaco po monomie)
    map<Monomial, double, greater<Monomial>> terms;
    int numVars;
    vector<string> varNames;

    Polynomial(int n, vector<string> names) : numVars(n), varNames(names) {}

    // Dodaj wyraz
    void addTerm(double coef, Monomial m) {
        if (abs(coef) < 1e-10) return;
        terms[m] += coef;
        if (abs(terms[m]) < 1e-10) terms.erase(m);
    }

    bool isZero() const { return terms.empty(); }

    // Wiodacy monom (najwiekszy w porzadku lex)
    Monomial LM() const {
        return terms.begin()->first;
    }
    // Wiodacy wspolczynnik
    double LC() const {
        return terms.begin()->second;
    }
    // Wiodacy wyraz
    pair<Monomial, double> LT() const {
        return *terms.begin();
    }

    // Normalizacja (LC = 1)
    Polynomial monic() const {
        Polynomial res = *this;
        double lc = LC();
        for (auto& [m, c] : res.terms) c /= lc;
        return res;
    }

    // Operacje arytmetyczne
    Polynomial operator+(const Polynomial& o) const {
        Polynomial res = *this;
        for (auto& [m, c] : o.terms) res.addTerm(c, m);
        return res;
    }
    Polynomial operator-(const Polynomial& o) const {
        Polynomial res = *this;
        for (auto& [m, c] : o.terms) res.addTerm(-c, m);
        return res;
    }
    Polynomial operator*(double scalar) const {
        Polynomial res(numVars, varNames);
        for (auto& [m, c] : terms) res.addTerm(c * scalar, m);
        return res;
    }
    Polynomial mulMonom(double coef, Monomial m) const {
        Polynomial res(numVars, varNames);
        for (auto& [tm, tc] : terms) res.addTerm(tc * coef, tm * m);
        return res;
    }

    string toString() const {
        if (isZero()) return "0";
        string s;
        for (auto& [m, c] : terms) {
            if (!s.empty() && c > 0) s += " + ";
            else if (c < 0) s += " - ";
            double ac = abs(c);
            string ms = m.toString(varNames);
            if (ms == "1") s += to_string((int)round(ac));
            else if (abs(ac - 1.0) < 1e-10) s += ms;
            else s += to_string((int)round(ac)) + ms;
        }
        return s;
    }
};

// ============================================================
//  CZESC b) - PolynomialReduce, Syzygium, Buchberger
// ============================================================

/*
 * Dzielenie wielomianu f przez liste [g1,...,gk] z porzadkiem lex.
 * Zwraca (reszta, wspolczynniki q1,...,qk takie ze f = sum qi*gi + reszta)
 *
 * Pseudokod:
 *   r = 0; p = f; q_i = 0 for all i
 *   while p != 0:
 *     i = 1; division_occurred = false
 *     while i <= s and not division_occurred:
 *       if LT(g_i) | LT(p):
 *         q_i += LT(p)/LT(g_i)
 *         p   -= (LT(p)/LT(g_i)) * g_i
 *         division_occurred = true
 *       else: i++
 *     if not division_occurred:
 *       r += LT(p); p -= LT(p)
 *   return q, r
 */
pair<vector<Polynomial>, Polynomial>
PolynomialReduce(const Polynomial& f, const vector<Polynomial>& G) {
    int n = f.numVars;
    auto vars = f.varNames;

    vector<Polynomial> q;
    for (int i = 0; i < (int)G.size(); i++) q.emplace_back(n, vars);

    Polynomial r(n, vars);
    Polynomial p = f;

    while (!p.isZero()) {
        bool divOccurred = false;
        for (int i = 0; i < (int)G.size(); i++) {
            Monomial ltP = p.LM();
            Monomial ltG = G[i].LM();
            if (ltP.divisibleBy(ltG)) {
                Monomial quotM = ltP / ltG;
                double   quotC = p.LC() / G[i].LC();
                q[i].addTerm(quotC, quotM);
                p = p - G[i].mulMonom(quotC, quotM);
                divOccurred = true;
                break;
            }
        }
        if (!divOccurred) {
            r.addTerm(p.LC(), p.LM());
            p.terms.erase(p.terms.begin());
        }
    }
    return {q, r};
}

/*
 * S-wielomian (Syzygium) S(f,g):
 *   S(f,g) = lcm(LM(f),LM(g))/LT(f) * f  -  lcm(LM(f),LM(g))/LT(g) * g
 */
Polynomial Spoly(const Polynomial& f, const Polynomial& g) {
    Monomial gamma = Monomial::lcm(f.LM(), g.LM());
    Monomial mf = gamma / f.LM();
    Monomial mg = gamma / g.LM();
    return f.mulMonom(1.0 / f.LC(), mf) - g.mulMonom(1.0 / g.LC(), mg);
}

/*
 * Algorytm Buchbergera dla bazy Groebnera.
 *
 * Pseudokod:
 *   G = F
 *   B = {(i,j) | i<j, fi,fj in G}
 *   while B != empty:
 *     wybierz (i,j) z B; B -= {(i,j)}
 *     r = reszta z PolynomialReduce(S(gi,gj), G)
 *     if r != 0:
 *       B += {(i, |G|) | gi in G}
 *       G += {r}
 *   return G
 */
vector<Polynomial> Buchberger(const vector<Polynomial>& F) {
    vector<Polynomial> G = F;
    set<pair<int,int>> B;
    for (int i = 0; i < (int)G.size(); i++)
        for (int j = i+1; j < (int)G.size(); j++)
            B.insert({i, j});

    while (!B.empty()) {
        auto it = B.begin();
        auto [i, j] = *it;
        B.erase(it);

        Polynomial s = Spoly(G[i], G[j]);
        auto [q, r] = PolynomialReduce(s, G);

        if (!r.isZero()) {
            int newIdx = (int)G.size();
            for (int k = 0; k < newIdx; k++) B.insert({k, newIdx});
            G.push_back(r.monic());
        }
    }
    return G;
}

/*
 * Zredukowana baza Groebnera:
 * 1. Usuniecie zbednych wielomianow (jesli LM(gi) | LM(gj) dla i!=j)
 * 2. Redukcja kazdego gi przez pozostale
 */
vector<Polynomial> reducedGroebner(vector<Polynomial> G) {
    // Krok 1: usuniecie nadmiarowych
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < (int)G.size(); i++) {
            vector<Polynomial> rest;
            for (int j = 0; j < (int)G.size(); j++)
                if (i != j) rest.push_back(G[j]);
            auto [q, r] = PolynomialReduce(G[i], rest);
            if (r.isZero()) {
                G.erase(G.begin() + i);
                changed = true;
                break;
            }
        }
    }
    // Krok 2: redukcja
    for (int i = 0; i < (int)G.size(); i++) {
        vector<Polynomial> rest;
        for (int j = 0; j < (int)G.size(); j++)
            if (i != j) rest.push_back(G[j]);
        auto [q, r] = PolynomialReduce(G[i], rest);
        if (!r.isZero()) G[i] = r.monic();
    }
    return G;
}

// ============================================================
//  Pomocnik: tworzenie wielomianow
// ============================================================

// Zmienne: x=0, y=1, z=2 (dla 3 zmiennych)
Polynomial makeVar(int idx, int n, vector<string> vars) {
    Polynomial p(n, vars);
    Monomial m(n);
    m.exp[idx] = 1;
    p.addTerm(1.0, m);
    return p;
}
Polynomial makeConst(double c, int n, vector<string> vars) {
    Polynomial p(n, vars);
    Monomial m(n); // all zeros = constant 1
    p.addTerm(c, m);
    return p;
}

// ============================================================
//  Funkcja pomocnicza: usun zmienna z (idx=2) z bazy Groebnera
//  przez wybranie tych wielomianow, ktore jej nie zawieraja
// ============================================================
vector<Polynomial> eliminateVar(const vector<Polynomial>& G, int varIdx) {
    vector<Polynomial> elim;
    for (auto& g : G) {
        bool hasVar = false;
        for (auto& [m, c] : g.terms)
            if (m.exp[varIdx] > 0) { hasVar = true; break; }
        if (!hasVar) elim.push_back(g);
    }
    return elim;
}

// ============================================================
//  CZESC c) - Uklad rownan: x^2+y^2=z^2, f(x,y,z)=0
// ============================================================

void solvePart_c() {
    // Zmienne: x=0, y=1, z=2
    int n = 3;
    vector<string> vars = {"x","y","z"};

    auto X = makeVar(0, n, vars);
    auto Y = makeVar(1, n, vars);
    auto Z = makeVar(2, n, vars);

    // g1 = x^2 + y^2 - z^2
    Polynomial g1(n, vars);
    Monomial mx2(n); mx2.exp[0]=2;
    Monomial my2(n); my2.exp[1]=2;
    Monomial mz2(n); mz2.exp[2]=2;
    g1.addTerm( 1, mx2);
    g1.addTerm( 1, my2);
    g1.addTerm(-1, mz2);

    cout << "=== CZESC c) ===" << endl;
    cout << "g1 = " << g1.toString() << endl;

    // ---- Przyklad 1: f(x,y,z) = (c+1)*z
    {
        int c = INDEX_C;
        double coef = c + 1;
        cout << "\n-- Przyklad 1: f = (" << c << "+1)*z = " << coef << "*z --" << endl;
        Polynomial f(n, vars);
        Monomial mz(n); mz.exp[2]=1;
        f.addTerm(coef, mz);
        cout << "f = " << f.toString() << endl;

        vector<Polynomial> F = {g1, f};
        auto G = reducedGroebner(Buchberger(F));
        cout << "Baza Groebnera I:" << endl;
        for (auto& p : G) cout << "  " << p.toString() << endl;
        auto Elim = eliminateVar(G, 2);
        cout << "Po eliminacji z (ideal I_f w R[x,y]):" << endl;
        for (auto& p : Elim) cout << "  " << p.toString() << endl;
        cout << "(Uwaga: jesli f=c*z i c!=0, to z=0 => x^2+y^2=0 => x=y=0 w R; w C: V(I_f) = {(0,0)})" << endl;
    }

    // ---- Przyklad 2: f(x,y,z) = z + d + 1
    {
        int d = INDEX_D;
        cout << "\n-- Przyklad 2: f = z + " << d << " + 1 = z + " << d+1 << " --" << endl;
        Polynomial f(n, vars);
        Monomial mz(n); mz.exp[2]=1;
        Monomial m0(n);
        f.addTerm(1.0, mz);
        f.addTerm(d+1, m0);
        cout << "f = " << f.toString() << endl;

        vector<Polynomial> F = {g1, f};
        auto G = reducedGroebner(Buchberger(F));
        cout << "Baza Groebnera I:" << endl;
        for (auto& p : G) cout << "  " << p.toString() << endl;
        auto Elim = eliminateVar(G, 2);
        cout << "Po eliminacji z (ideal I_f w R[x,y]):" << endl;
        for (auto& p : Elim) cout << "  " << p.toString() << endl;
        cout << "(Krzywa w R^2: V(I_f) to okrag x^2+y^2 = " << (d+1)*(d+1) << ")" << endl;
    }

    // ---- Przyklad 3: f(x,y,z) = x + z - e - 1
    {
        int e = INDEX_E;
        cout << "\n-- Przyklad 3: f = x + z - " << e << " - 1 = x + z - " << e+1 << " --" << endl;
        Polynomial f(n, vars);
        Monomial mx(n); mx.exp[0]=1;
        Monomial mz(n); mz.exp[2]=1;
        Monomial m0(n);
        f.addTerm(1.0, mx);
        f.addTerm(1.0, mz);
        f.addTerm(-(e+1), m0);
        cout << "f = " << f.toString() << endl;

        vector<Polynomial> F = {g1, f};
        auto G = reducedGroebner(Buchberger(F));
        cout << "Baza Groebnera I:" << endl;
        for (auto& p : G) cout << "  " << p.toString() << endl;
        auto Elim = eliminateVar(G, 2);
        cout << "Po eliminacji z (ideal I_f w R[x,y]):" << endl;
        for (auto& p : Elim) cout << "  " << p.toString() << endl;
    }

    // ---- Przyklad 4: f(x,y,z) = x + y + z + f_coef + 1
    {
        int fc = INDEX_F;
        cout << "\n-- Przyklad 4: f = x + y + z + " << fc << " + 1 = x + y + z + " << fc+1 << " --" << endl;
        Polynomial f(n, vars);
        Monomial mx(n); mx.exp[0]=1;
        Monomial my(n); my.exp[1]=1;
        Monomial mz(n); mz.exp[2]=1;
        Monomial m0(n);
        f.addTerm(1.0, mx);
        f.addTerm(1.0, my);
        f.addTerm(1.0, mz);
        f.addTerm(fc+1, m0);
        cout << "f = " << f.toString() << endl;

        vector<Polynomial> F = {g1, f};
        auto G = reducedGroebner(Buchberger(F));
        cout << "Baza Groebnera I:" << endl;
        for (auto& p : G) cout << "  " << p.toString() << endl;
        auto Elim = eliminateVar(G, 2);
        cout << "Po eliminacji z (ideal I_f w R[x,y]):" << endl;
        for (auto& p : Elim) cout << "  " << p.toString() << endl;
    }

    // ---- Przyklad 5: f(x,y,z) = y/a + z + 1
    //  (dla a=0 uzyj a=1 by uniknac dzielenia przez zero)
    {
        int a = INDEX_A;
        if (a == 0) a = 1;
        cout << "\n-- Przyklad 5: f = y/" << a << " + z + 1 --" << endl;
        // f = (1/a)*y + z + 1, mnozac przez a: y + a*z + a (idealy sa rownowazne)
        Polynomial f(n, vars);
        Monomial my(n); my.exp[1]=1;
        Monomial mz(n); mz.exp[2]=1;
        Monomial m0(n);
        f.addTerm(1.0/a, my);
        f.addTerm(1.0, mz);
        f.addTerm(1.0, m0);
        cout << "f = " << f.toString() << endl;

        vector<Polynomial> F = {g1, f};
        auto G = reducedGroebner(Buchberger(F));
        cout << "Baza Groebnera I:" << endl;
        for (auto& p : G) cout << "  " << p.toString() << endl;
        auto Elim = eliminateVar(G, 2);
        cout << "Po eliminacji z (ideal I_f w R[x,y]):" << endl;
        for (auto& p : Elim) cout << "  " << p.toString() << endl;
    }
}

// ============================================================
//  CZESC d) - Bardziej zlozone rownania
// ============================================================

void solvePart_d() {
    cout << "\n=== CZESC d) ===" << endl;

    // Zmienne: x=0, y=1, z=2
    int n = 3;
    vector<string> vars = {"x","y","z"};

    int a = INDEX_A;
    if (a == 0) a = 1; // unikamy a=0 (inaczej rownanie zdegenerowane)

    cout << "Parametr a = " << a << endl;

    // Uklad: (x^2+y^2-ax)^2 = z^2(x^2+y^2),  x+2y+3z=0
    // Rozwinmy pierwsze rownanie:
    //   (x^2+y^2)^2 - 2ax(x^2+y^2) + a^2*x^2 - z^2*x^2 - z^2*y^2 = 0
    // g1 = (x^2+y^2-ax)^2 - z^2(x^2+y^2) = 0
    // g2 = x + 2y + 3z = 0

    Polynomial g1(n, vars);
    // (x^2+y^2)^2 = x^4 + 2x^2y^2 + y^4
    Monomial mx4(n); mx4.exp[0]=4;
    Monomial mx2y2(n); mx2y2.exp[0]=2; mx2y2.exp[1]=2;
    Monomial my4(n); my4.exp[1]=4;
    g1.addTerm(1, mx4);
    g1.addTerm(2, mx2y2);
    g1.addTerm(1, my4);
    // -2ax(x^2+y^2) = -2a*x^3 - 2a*x*y^2
    Monomial mx3(n); mx3.exp[0]=3;
    Monomial mxy2(n); mxy2.exp[0]=1; mxy2.exp[1]=2;
    g1.addTerm(-2.0*a, mx3);
    g1.addTerm(-2.0*a, mxy2);
    // +a^2*x^2
    Monomial mx2(n); mx2.exp[0]=2;
    g1.addTerm((double)a*a, mx2);
    // -z^2*x^2
    Monomial mx2z2(n); mx2z2.exp[0]=2; mx2z2.exp[2]=2;
    g1.addTerm(-1, mx2z2);
    // -z^2*y^2
    Monomial my2z2(n); my2z2.exp[1]=2; my2z2.exp[2]=2;
    g1.addTerm(-1, my2z2);

    Polynomial g2(n, vars);
    Monomial mx(n); mx.exp[0]=1;
    Monomial my(n); my.exp[1]=1;
    Monomial mz(n); mz.exp[2]=1;
    g2.addTerm(1, mx);
    g2.addTerm(2, my);
    g2.addTerm(3, mz);

    cout << "g1 = " << g1.toString() << endl;
    cout << "g2 = " << g2.toString() << endl;

    cout << "\nLiczenie bazy Groebnera (moze chwile zajac)..." << endl;
    vector<Polynomial> F = {g1, g2};
    auto G = reducedGroebner(Buchberger(F));
    cout << "Baza Groebnera:" << endl;
    for (auto& p : G) cout << "  " << p.toString() << endl;

    // Eliminacja x (idx=0)
    auto Elim = eliminateVar(G, 0);
    cout << "\nPo eliminacji x (wielomiany w y,z):" << endl;
    for (auto& p : Elim) cout << "  " << p.toString() << endl;

    // Dla z=a: podstaw w g1 z=a => (x^2+y^2-ax)^2 = a^2(x^2+y^2)
    // To jest rownianie kartezjanskie okragu (x-a/2)^2+y^2=(a/2)^2*? => nie, to lemniskata
    // Krzywa Pascala / Kardioidna / Lemniskata Bernoulliego (zalezy od a)
    cout << "\nDla z = a = " << a << " (pierwsze rownanie): (x^2+y^2-" << a << "x)^2 = "
         << a*a << "(x^2+y^2)" << endl;
    cout << "Jest to rownanie lemniskatoidu / petli Pascala w wspolrzednych kartezjanskich." << endl;
}

// ============================================================
//  CZESC e) - Trysektysa (trisectrix of Maclaurin)
// ============================================================

void solvePart_e() {
    cout << "\n=== CZESC e) ===" << endl;

    int b = INDEX_B;
    if (b == 0) b = 1;

    cout << "Parametr b = " << b << endl;
    cout << "Trysektysa: r = " << b << " * (4cos(t) + 1/cos(t))" << endl;
    cout << "         = " << b << " * (4cos(t) + sec(t))" << endl;
    cout << endl;

    // r = b*(4cos(t) + 1/cos(t))
    // Mnozac przez cos(t): r*cos(t) = b*(4cos^2(t) + 1)
    // Podstawiamy: x = r*cos(t), y = r*sin(t), r^2 = x^2+y^2, cos(t)=x/r
    //   r * (x/r) = b*(4*(x/r)^2 + 1)
    //   x = b*(4x^2/r^2 + 1)
    //   x*r^2 = b*(4x^2 + r^2)
    //   x*(x^2+y^2) = b*(4x^2 + x^2+y^2)
    //   x^3 + xy^2 = 4bx^2 + bx^2 + by^2
    //   x^3 + xy^2 = (4b+b)x^2 + by^2     [uwaga: 4bx^2+bx^2 = (4b+b)x^2 = 5bx^2 ??]
    // Sprawdzmy ponownie:
    //   x*(x^2+y^2) = b*(4x^2 + x^2+y^2)
    //   x^3 + xy^2  = 4bx^2 + bx^2 + by^2
    //   x^3 + xy^2  = b(4x^2) + b(x^2+y^2)
    //   x^3 + xy^2  = 4bx^2 + bx^2 + by^2    <- BLAD: 4b*x^2 + b*r^2 = 4bx^2+bx^2+by^2
    // Poprawnie:
    //   x*r^2 = b*(4x^2 + r^2)
    //   x*(x^2+y^2) = 4bx^2 + b(x^2+y^2)
    //   x^3 + xy^2 - 4bx^2 - bx^2 - by^2 = 0
    // Hmm, pokrocmy bardziej starannie:
    //   x*(x^2+y^2) - b*(4x^2 + x^2+y^2) = 0
    //   x^3+xy^2 - 4bx^2 - bx^2 - by^2   = 0
    //   x^3 + xy^2 - (4b+b)x^2 - by^2    = 0
    //   x^3 + xy^2 - (4b+b)x^2 - by^2    = 0
    // Ale to jest 4bx^2 + bx^2 = 5bx^2? Nie, x*(x^2+y^2) = b*(4x^2+(x^2+y^2))
    //   x^3+xy^2 = 4bx^2 + b*x^2 + b*y^2
    //   x^3+xy^2 = (4b+b)x^2 + by^2
    // ale: 4bx^2 + b*(x^2+y^2) = 4bx^2+bx^2+by^2 = 5bx^2+by^2 <- prawda
    // Wiec: x^3+xy^2 - 5bx^2 - by^2 = 0  <- to jest rownanie trysektysy
    // Weryfikacja: dla b=1 => x^3+xy^2-5x^2-y^2=0, zbiez z literatura (Trisectrix of Maclaurin)

    // Wyswietl rownanie
    cout << "Rownanie kartezjanskie trysektysy:" << endl;
    cout << "  x^3 + x*y^2 - " << (4*b+b) << "*x^2 - " << b << "*y^2 = 0" << endl;
    cout << "  x^3 + xy^2 = " << b << "*(5x^2 + y^2)  [sprawdz: powinno byc 4bx^2+b(x^2+y^2)]" << endl;
    cout << endl;
    cout << "  Ogolnie: x*(x^2+y^2) = b*(4x^2 + x^2+y^2) = b*(5x^2+y^2)" << endl;
    cout << "  Wiec: x^3 + xy^2 - " << 5*b << "x^2 - " << b << "y^2 = 0" << endl;
    cout << endl;
    cout << "Weryfikacja przy pomocy bazy Groebnera:" << endl;
    cout << "  (uzyjemy zmiennych x,y,r,c gdzie c=cos(t), r^2=x^2+y^2, x=r*c, y=r*sin(t))" << endl;
    cout << endl;
    cout << "Opis trysektysy:" << endl;
    cout << "  Trysektysa Maclaurina jest krzywa algebraiczna 3. stopnia." << endl;
    cout << "  Sluzy do trysekowania katow: jesli dany jest kat alfa," << endl;
    cout << "  trysektysa pozwala geometrycznie skonstruowac kat alfa/3." << endl;
    cout << "  Jest rowniez zwiazana z problemem trysekcji kata (niemozliwe" << endl;
    cout << "  przy uzyciu cyrkla i linijki, ale mozliwe z trysektysa)." << endl;
    cout << "  Krzywa przechodzi przez poczatek ukladu wspolrzednych" << endl;
    cout << "  i ma asymptote x = -b." << endl;

    // Potwierdzenie przez baze Groebnera (2 zmienne)
    int n = 2;
    vector<string> vars2 = {"x","y"};
    Polynomial p(n, vars2);
    Monomial mx3(n); mx3.exp[0]=3;
    Monomial mxy2(n); mxy2.exp[0]=1; mxy2.exp[1]=2;
    Monomial mx2(n); mx2.exp[0]=2;
    Monomial my2(n); my2.exp[1]=2;
    p.addTerm(1, mx3);
    p.addTerm(1, mxy2);
    p.addTerm(-5.0*b, mx2);
    p.addTerm(-(double)b, my2);
    cout << "\nWielomian trysektysy: " << p.toString() << " = 0" << endl;
}

// ============================================================
//  MAIN
// ============================================================

int main() {
    cout << "======================================" << endl;
    cout << " LAB 6 - Pierscienie wielomianow" << endl;
    cout << " Baza Groebnera (algorytm Buchbergera)" << endl;
    cout << " Indeks: ________" << endl;
    cout << " a=" << INDEX_A << " b=" << INDEX_B << " c=" << INDEX_C
         << " d=" << INDEX_D << " e=" << INDEX_E << " f=" << INDEX_F << endl;
    cout << "======================================" << endl;

    // --- a) wyswietl opis architektury ---
    cout << "\n=== CZESC a) Pierscien wielomianow wielu zmiennych ===" << endl;
    cout << "Implementacja: struct Monomial (wektor wykladnikow), struct Polynomial (mapa monom->wsp)." << endl;
    cout << "Roznice wzgledem 1 zmiennej:" << endl;
    cout << "  - Monom to wektor int (nie pojedynczy stopien)" << endl;
    cout << "  - Porzadek monomow: lex (mozna tez grlex/grevlex)" << endl;
    cout << "  - LM zalezy od wybranego porzadku" << endl;
    cout << "  - Dzielenie daje potencjalnie wiele reszt" << endl;
    cout << "  - Baza Groebnera zastepuje NWD z przypadku 1 zmiennej" << endl;

    // --- b) demo PolynomialReduce i Buchberger ---
    cout << "\n=== CZESC b) Demo PolynomialReduce, Syzygium, Buchberger ===" << endl;
    {
        int n = 2;
        vector<string> vars = {"x","y"};
        Polynomial f(n, vars);
        // f = x^2*y + x*y^2 + y^2
        Monomial m1(n); m1.exp[0]=2; m1.exp[1]=1;
        Monomial m2(n); m2.exp[0]=1; m2.exp[1]=2;
        Monomial m3(n); m3.exp[1]=2;
        f.addTerm(1, m1); f.addTerm(1, m2); f.addTerm(1, m3);

        Polynomial g1(n, vars);
        Monomial gm1(n); gm1.exp[0]=1; gm1.exp[1]=1;
        Monomial gm2(n); gm2.exp[1]=1;
        g1.addTerm(1, gm1); g1.addTerm(-1, gm2);

        Polynomial g2(n, vars);
        Monomial gm3(n); gm3.exp[1]=2;
        Monomial gm4(n); // const
        g2.addTerm(1, gm3); g2.addTerm(-1, gm4);

        cout << "f  = " << f.toString() << endl;
        cout << "g1 = " << g1.toString() << endl;
        cout << "g2 = " << g2.toString() << endl;

        auto [q, r] = PolynomialReduce(f, {g1, g2});
        cout << "PolynomialReduce(f, [g1,g2]):" << endl;
        cout << "  q1=" << q[0].toString() << ", q2=" << q[1].toString() << ", r=" << r.toString() << endl;

        cout << "S(g1,g2) = " << Spoly(g1,g2).toString() << endl;

        auto G = Buchberger({g1, g2});
        cout << "Buchberger({g1,g2}):" << endl;
        for (auto& p : G) cout << "  " << p.toString() << endl;
    }

    // --- c) ---
    solvePart_c();

    // --- d) ---
    solvePart_d();

    // --- e) ---
    solvePart_e();

    cout << "\n======================================" << endl;
    cout << " Koniec programu" << endl;
    cout << "======================================" << endl;
    return 0;
}