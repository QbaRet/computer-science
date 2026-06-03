#include <bits/stdc++.h>
using namespace std;

// ============================================================
// a) Struktura wielomianu wielu zmiennych
// ============================================================

struct Monomial {
    vector<int> exp;
    double coef;

    int numVars() const { return (int)exp.size(); }

    int totalDeg() const {
        return accumulate(exp.begin(), exp.end(), 0);
    }

    bool isZero() const { return abs(coef) < 1e-12; }

    Monomial operator*(const Monomial& o) const {
        Monomial res;
        res.coef = coef * o.coef;
        res.exp.resize(exp.size());
        for (int i = 0; i < (int)exp.size(); i++)
            res.exp[i] = exp[i] + o.exp[i];
        return res;
    }

    Monomial operator/(const Monomial& o) const {
        Monomial res;
        res.coef = coef / o.coef;
        res.exp.resize(exp.size());
        for (int i = 0; i < (int)exp.size(); i++)
            res.exp[i] = exp[i] - o.exp[i];
        return res;
    }

    bool divisibleBy(const Monomial& o) const {
        for (int i = 0; i < (int)exp.size(); i++)
            if (exp[i] < o.exp[i]) return false;
        return true;
    }

    string toString(const vector<string>& vars) const {
        if (isZero()) return "0";
        ostringstream oss;
        bool printed = false;
        if (abs(coef - 1.0) > 1e-12 || totalDeg() == 0) {
            oss << fixed << setprecision(4) << coef;
            printed = true;
        }
        for (int i = 0; i < (int)vars.size(); i++) {
            if (exp[i] == 1) { oss << vars[i]; printed = true; }
            else if (exp[i] > 1) { oss << vars[i] << "^" << exp[i]; printed = true; }
        }
        if (!printed) oss << "1";
        return oss.str();
    }
};

// ============================================================
// b) Porządki na jednomianach
// ============================================================

// LEX       – leksykograficzny (standardowy lub permutowany)
// GRADED_LEX – najpierw stopień łączny, potem lex (= glex / grlex)
enum MonomialOrder { LEX, PERM_LEX, GRADED_LEX };

/*
 * Różnice między porządkami:
 *
 * Standardowy Lex (perm = {0,1,...,n-1}):
 *   Porównuje wykładniki zaczynając od pierwszej zmiennej.
 *   x^2 > xy > x > y^2 > y > 1 (dla x,y)
 *
 * Permutowany Lex (dowolna permutacja):
 *   Jak Lex, ale zmienne brane są w kolejności zadanej przez perm[].
 *   Zmiana permutacji zmienia wynik redukcji (punkt e zadania).
 *
 * GradedLex (glex):
 *   Najpierw porównuje stopień łączny, przy równym stopniu – Lex.
 *   x^2 > xy > y^2 > x > y > 1 (dla x,y)
 *   Używany np. w algorytmie Buchbergera / bazach Gröbnera.
 */

bool lexLess(const Monomial& a, const Monomial& b,
             const vector<int>& perm) {
    for (int idx : perm) {
        if (a.exp[idx] != b.exp[idx])
            return a.exp[idx] < b.exp[idx];
    }
    return false;
}

bool gradedLexLess(const Monomial& a, const Monomial& b,
                   const vector<int>& perm) {
    int da = a.totalDeg(), db = b.totalDeg();
    if (da != db) return da < db;
    return lexLess(a, b, perm);
}

bool monomialLess(const Monomial& a, const Monomial& b,
                  MonomialOrder ord, const vector<int>& perm) {
    switch (ord) {
        case LEX:
        case PERM_LEX:   return lexLess(a, b, perm);
        case GRADED_LEX: return gradedLexLess(a, b, perm);
    }
    return false;
}

// ============================================================
// Wielomian
// ============================================================

struct Polynomial {
    vector<Monomial> terms;
    int n;
    vector<string> vars;

    Polynomial(int numVars, vector<string> varNames)
        : n(numVars), vars(varNames) {}

    void addMonomial(Monomial m) {
        if (!m.isZero()) terms.push_back(m);
    }

    void normalize(MonomialOrder ord, const vector<int>& perm) {
        map<vector<int>, double> acc;
        for (auto& m : terms) acc[m.exp] += m.coef;
        terms.clear();
        for (auto& [e, c] : acc) {
            if (abs(c) > 1e-12) {
                Monomial m; m.exp = e; m.coef = c;
                terms.push_back(m);
            }
        }
        sort(terms.begin(), terms.end(),
             [&](const Monomial& a, const Monomial& b) {
                 return monomialLess(b, a, ord, perm);
             });
    }

    bool isZero() const { return terms.empty(); }
    const Monomial& LT() const { return terms[0]; }
    double LC() const { return terms[0].coef; }

    string toString() const {
        if (terms.empty()) return "0";
        string s;
        for (int i = 0; i < (int)terms.size(); i++) {
            if (i > 0 && terms[i].coef > 0) s += " + ";
            else if (i > 0 && terms[i].coef < 0) s += " ";
            s += terms[i].toString(vars);
        }
        return s;
    }

    Polynomial operator+(const Polynomial& o) const {
        Polynomial res(n, vars);
        res.terms = terms;
        for (auto& m : o.terms) res.terms.push_back(m);
        return res;
    }

    Polynomial operator-(const Polynomial& o) const {
        Polynomial res(n, vars);
        res.terms = terms;
        for (auto m : o.terms) { m.coef = -m.coef; res.terms.push_back(m); }
        return res;
    }

    Polynomial mulMonomial(const Monomial& m) const {
        Polynomial res(n, vars);
        for (auto& t : terms) res.terms.push_back(t * m);
        return res;
    }
};

// ============================================================
// c) PolynomialReduce
//
// Algorytm (Becker & Weispfenning, s. 62):
//   Dopóki f ≠ 0:
//     Szukaj gi takie, że LT(gi) | LT(f).
//     Jeśli znaleziono:
//       q = LT(f) / LT(gi)
//       alpha_i += q
//       f -= q * gi
//     W przeciwnym razie:
//       r += LT(f);  f -= LT(f)
//
// Niezmiennik: f_orig = sum(alpha_i * gi) + r
// ============================================================

struct ReduceResult {
    vector<Polynomial> alpha;
    Polynomial r;
};

ReduceResult PolynomialReduce(
        Polynomial f,
        const vector<Polynomial>& G,
        MonomialOrder ord,
        const vector<int>& perm)
{
    int k = (int)G.size();
    int nv = f.n;

    ReduceResult res { vector<Polynomial>(k, Polynomial(nv, f.vars)),
                       Polynomial(nv, f.vars) };

    f.normalize(ord, perm);

    while (!f.isZero()) {
        bool divOccurred = false;

        for (int i = 0; i < k && !divOccurred; i++) {
            Polynomial gi = G[i];
            gi.normalize(ord, perm);

            if (!f.isZero() && f.LT().divisibleBy(gi.LT())) {
                Monomial q = f.LT() / gi.LT();

                Polynomial qPoly(nv, f.vars);
                qPoly.addMonomial(q);
                qPoly.normalize(ord, perm);

                res.alpha[i] = (res.alpha[i] + qPoly);
                res.alpha[i].normalize(ord, perm);

                f = f - gi.mulMonomial(q);
                f.normalize(ord, perm);

                divOccurred = true;
            }
        }

        if (!divOccurred && !f.isZero()) {
            Polynomial ltPoly(nv, f.vars);
            ltPoly.addMonomial(f.LT());
            res.r = res.r + ltPoly;
            res.r.normalize(ord, perm);

            Monomial neg = f.LT(); neg.coef = -neg.coef;
            Polynomial ltNeg(nv, f.vars);
            ltNeg.addMonomial(neg);
            f = f + ltNeg;
            f.normalize(ord, perm);
        }
    }

    return res;
}

// ============================================================
// Pomocnicze
// ============================================================

void printResult(const string& fname,
                 const vector<string>& gnames,
                 const ReduceResult& res)
{
    cout << fname << " = ";
    bool any = false;
    for (int i = 0; i < (int)res.alpha.size(); i++) {
        string a = res.alpha[i].toString();
        if (a != "0") {
            cout << "(" << a << ") * " << gnames[i] << " + ";
            any = true;
        }
    }
    if (!any) cout << "0 + ";
    cout << "r,  r = " << res.r.toString() << "\n";
}

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

struct IndexDigits {
    array<int, 6> digits{};
    int operator[](size_t idx) const { return digits[idx]; }
};

bool readIndexDigits(IndexDigits& indexDigits) {
    cout << "Podaj 6 cyfr numeru indeksu: ";
    string index;
    if (!(cin >> index)) return false;
    if (index.size() != 6 ||
        !all_of(index.begin(), index.end(), [](unsigned char c){ return isdigit(c); })) {
        cerr << "Blad: numer indeksu musi skladac sie dokladnie z 6 cyfr.\n";
        return false;
    }
    for (size_t i = 0; i < 6; ++i)
        indexDigits.digits[i] = index[i] - '0';
    return true;
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    cout << fixed << setprecision(4);

    IndexDigits idx;
    if (!readIndexDigits(idx)) return 1;

    cout << "Numer indeksu: ";
    for (int d : idx.digits) cout << d;
    cout << "\n\n";

    // --------------------------------------------------------
    // d) Ćwiczenie 37 – GradedLex, 3 zmienne x,y,z
    //    f  = x^3 - x^2*y - x^2*z
    //    g1 = x^2*y - z
    //    g2 = x*y - 1
    // --------------------------------------------------------
    int n = 3;
    vector<string> vars = {"x","y","z"};
    vector<int> permStd = {0, 1, 2};   // x > y > z

    // f = x^3 - x^2*y - x^2*z
    Polynomial f = makePoly(n, vars, {
        { 1.0, {3,0,0}},
        {-1.0, {2,1,0}},
        {-1.0, {2,0,1}}
    });

    // g1 = x^2*y - z
    Polynomial g1 = makePoly(n, vars, {
        { 1.0, {2,1,0}},
        {-1.0, {0,0,1}}
    });

    // g2 = x*y - 1
    Polynomial g2 = makePoly(n, vars, {
        { 1.0, {1,1,0}},
        {-1.0, {0,0,0}}
    });

    cout << "=== Cw. 37  (GradedLex, x > y > z) ===\n";
    cout << "f  = " << f.toString()  << "\n";
    cout << "g1 = " << g1.toString() << "\n";
    cout << "g2 = " << g2.toString() << "\n\n";

    cout << "--- 1. Redukcja przez (g1, g2) ---\n";
    auto res1 = PolynomialReduce(f, {g1, g2}, GRADED_LEX, permStd);
    printResult("f", {"g1","g2"}, res1);

    cout << "\n--- 2. Redukcja przez (g2, g1) ---\n";
    auto res2 = PolynomialReduce(f, {g2, g1}, GRADED_LEX, permStd);
    printResult("f", {"g2","g1"}, res2);

    cout << "\n--- 3. Czy r1 - r2 nalezy do <g1,g2>? ---\n";
    string r1s = res1.r.toString();
    string r2s = res2.r.toString();
    cout << "r1 = " << r1s << "\n";
    cout << "r2 = " << r2s << "\n";
    // Redukcja r1-r2 przez {g1,g2}
    Polynomial diff = res1.r - res2.r;
    diff.normalize(GRADED_LEX, permStd);
    auto resD = PolynomialReduce(diff, {g1, g2}, GRADED_LEX, permStd);
    cout << "Reszta z redukcji (r1-r2) przez {g1,g2}: " << resD.r.toString() << "\n";
    if (resD.r.isZero())
        cout << "=> r1 - r2 NALEZY do <g1,g2>.\n";
    else
        cout << "=> r1 - r2 NIE nalezy do <g1,g2> (reszta niezerowa).\n";


    // --------------------------------------------------------
    // e) Wielomian h z numeru indeksu, 3 rozne porzadki Lex
    //    h(x,y,z) = x^a*y^b - y^c*z^d + x^e*z^f
    //    a=idx[0], b=idx[1], c=idx[2], d=idx[3], e=idx[4], f=idx[5]
    // --------------------------------------------------------
    cout << "\n=== Czesc e): wielomian z numeru indeksu, 3 porzadki ===\n";

    Polynomial h = makePoly(n, vars, {
        { 1.0, {idx[0], idx[1],      0}},
        {-1.0, {     0, idx[2], idx[3]}},
        { 1.0, {idx[4],      0, idx[5]}}
    });
    cout << "h = x^" << idx[0] << "*y^" << idx[1]
         << " - y^" << idx[2] << "*z^" << idx[3]
         << " + x^" << idx[4] << "*z^" << idx[5] << "\n\n";

    // Permutacje (porzadki leksykograficzne)
    vector<int> pXYZ = {0, 1, 2};  // x > y > z
    vector<int> pYZX = {1, 2, 0};  // y > z > x
    vector<int> pZXY = {2, 0, 1};  // z > x > y

    // Przeszukaj kandydatow na G
    vector<vector<Polynomial>> candidates = {
        {
            makePoly(n, vars, {{ 1.0,{2,1,0}},{-1.0,{0,0,1}}}),  // x^2*y - z
            makePoly(n, vars, {{ 1.0,{1,1,0}},{-1.0,{0,0,0}}})   // x*y - 1
        },
        {
            makePoly(n, vars, {{ 1.0,{1,0,0}},{-1.0,{0,1,0}}}),  // x - y
            makePoly(n, vars, {{ 1.0,{1,0,0}},{-1.0,{0,0,1}}})   // x - z
        },
        {
            makePoly(n, vars, {{ 1.0,{1,0,0}},{-1.0,{0,1,0}}}),  // x - y
            makePoly(n, vars, {{ 1.0,{0,1,0}},{-1.0,{0,0,1}}})   // y - z
        },
        {
            makePoly(n, vars, {{ 1.0,{1,0,0}},{-1.0,{0,0,1}}}),  // x - z
            makePoly(n, vars, {{ 1.0,{0,1,0}},{-1.0,{0,0,1}}}),  // y - z
            makePoly(n, vars, {{ 1.0,{1,1,0}},{-1.0,{0,0,1}}})   // xy - z
        },
        {
            makePoly(n, vars, {{ 1.0,{1,0,0}},{-1.0,{0,1,0}}}),  // x - y
            makePoly(n, vars, {{ 1.0,{0,1,0}},{-1.0,{0,0,1}}}),  // y - z
            makePoly(n, vars, {{ 1.0,{1,0,0}},{-1.0,{0,0,1}}})   // x - z
        },
    };

    bool found = false;
    for (size_t ci = 0; ci < candidates.size() && !found; ++ci) {
        auto r1e = PolynomialReduce(h, candidates[ci], LEX, pXYZ);
        auto r2e = PolynomialReduce(h, candidates[ci], LEX, pYZX);
        auto r3e = PolynomialReduce(h, candidates[ci], LEX, pZXY);

        string s1 = r1e.r.toString();
        string s2 = r2e.r.toString();
        string s3 = r3e.r.toString();

        if (s1 != s2 && s1 != s3 && s2 != s3) {
            cout << "Znaleziony ciag G (kandydat " << ci << "):\n";
            for (size_t j = 0; j < candidates[ci].size(); ++j)
                cout << "  g" << (j+1) << " = " << candidates[ci][j].toString() << "\n";
            cout << "Lex (x>y>z):  r = " << s1 << "\n";
            cout << "Lex (y>z>x):  r = " << s2 << "\n";
            cout << "Lex (z>x>y):  r = " << s3 << "\n";
            found = true;
        }
    }

    if (!found) {
        // Jesli powyzsze nie zadziala – pokaz przynajmniej rozne permutacje
        cout << "Automatyczne szukanie nie znalezlo G z 3 roznyymi resztami.\n";
        cout << "Pokazuje wyniki dla domyslnego G = {x-y, y-z}:\n";
        auto cand = vector<Polynomial>{
            makePoly(n, vars, {{ 1.0,{2,1,0}},{-1.0,{0,0,1}}}),  // x^2*y - z
            makePoly(n, vars, {{ 1.0,{1,1,0}},{-1.0,{0,0,0}}})   // x*y - 1
        };
        auto r1e = PolynomialReduce(h, cand, LEX, pXYZ);
        auto r2e = PolynomialReduce(h, cand, LEX, pYZX);
        auto r3e = PolynomialReduce(h, cand, LEX, pZXY);
        cout << "Lex (x>y>z):  r = " << r1e.r.toString() << "\n";
        cout << "Lex (y>z>x):  r = " << r2e.r.toString() << "\n";
        cout << "Lex (z>x>y):  r = " << r3e.r.toString() << "\n";
    }



    return 0;
}