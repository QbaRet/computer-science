#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <stdexcept>

using namespace std;

// Alias dla wykładników, np. {2, 0, 1} dla x^2 * z
typedef vector<int> Exponents;

struct Term {
    double coeff;
    Exponents exp;
};

// =======================================================================
// b) PORZĄDKI NA JEDNOMIANACH
// =======================================================================

struct Lex {
    bool operator()(const Term& t1, const Term& t2) const {
        for (size_t i = 0; i < t1.exp.size(); ++i) {
            if (t1.exp[i] != t2.exp[i]) 
                return t1.exp[i] > t2.exp[i];
        }
        return false;
    }
};

struct PermutedLex {
    vector<int> perm; // np. {1, 0, 2} oznacza y > x > z
    PermutedLex(vector<int> p) : perm(p) {}
    
    bool operator()(const Term& t1, const Term& t2) const {
        for (int p : perm) {
            if (t1.exp[p] != t2.exp[p])
                return t1.exp[p] > t2.exp[p];
        }
        return false;
    }
};

struct GrLex {
    bool operator()(const Term& t1, const Term& t2) const {
        int deg1 = accumulate(t1.exp.begin(), t1.exp.end(), 0);
        int deg2 = accumulate(t2.exp.begin(), t2.exp.end(), 0);
        
        if (deg1 != deg2) return deg1 > deg2;
        
        Lex lex;
        return lex(t1, t2);
    }
};

// =======================================================================
// a) STRUKTURA OBSŁUGUJĄCA WIELOMIANY WIELU ZMIENNYCH RZECZYWISTYCH
// =======================================================================
class MultiPoly {
public:
    vector<Term> terms;
    int num_vars;

    MultiPoly(int vars = 0) : num_vars(vars) {}

    void add_term(double coeff, Exponents exp) {
        if (abs(coeff) > 1e-5) terms.push_back({coeff, exp});
    }

    template <typename Comparator>
    void normalize(Comparator comp) {
        if (terms.empty()) return;
        
        sort(terms.begin(), terms.end(), comp);
        
        vector<Term> reduced;
        reduced.push_back(terms[0]);
        for (size_t i = 1; i < terms.size(); ++i) {
            if (terms[i].exp == reduced.back().exp) {
                reduced.back().coeff += terms[i].coeff;
            } else {
                reduced.push_back(terms[i]);
            }
        }
        
        terms.clear();
        for (const auto& t : reduced) {
            if (abs(t.coeff) > 1e-5) terms.push_back(t);
        }
    }

    bool is_zero() const { return terms.empty(); }

    Term leading_term() const {
        if (is_zero()) throw runtime_error("Wielomian zerowy nie ma LT");
        return terms[0];
    }

    MultiPoly operator-(const MultiPoly& other) const {
        MultiPoly res(num_vars);
        res.terms = terms;
        for (const auto& t : other.terms) res.add_term(-t.coeff, t.exp);
        return res;
    }

    MultiPoly multiply_by_term(const Term& t) const {
        MultiPoly res(num_vars);
        for (const auto& my_t : terms) {
            Exponents new_exp(num_vars);
            for(int i = 0; i < num_vars; ++i) new_exp[i] = my_t.exp[i] + t.exp[i];
            res.add_term(my_t.coeff * t.coeff, new_exp);
        }
        return res;
    }

    void print(const vector<string>& vars) const {
        if (is_zero()) { cout << "0"; return; }
        bool first = true;
        for (const auto& t : terms) {
            if (!first && t.coeff > 0) cout << " + ";
            if (t.coeff < 0) { if (first) cout << "-"; else cout << " - "; }
            
            double val = abs(t.coeff);
            bool all_zeros = true;
            for(int e : t.exp) if(e != 0) { all_zeros = false; break; }
            
            if (val != 1.0 || all_zeros) cout << val;
            for (size_t i = 0; i < t.exp.size(); ++i) {
                if (t.exp[i] > 0) {
                    cout << vars[i];
                    if (t.exp[i] > 1) cout << "^" << t.exp[i];
                }
            }
            first = false;
        }
    }
};

// Zmiana nazwy z 'divides' na 'term_divides', aby uniknąć kolizji z std::divides
bool term_divides(const Term& t1, const Term& t2) {
    for (size_t i = 0; i < t1.exp.size(); ++i) {
        if (t2.exp[i] > t1.exp[i]) return false;
    }
    return true;
}

Term divide_terms(const Term& t1, const Term& t2) {
    Exponents res_exp(t1.exp.size());
    for (size_t i = 0; i < res_exp.size(); ++i) {
        res_exp[i] = t1.exp[i] - t2.exp[i];
    }
    return {t1.coeff / t2.coeff, res_exp};
}

// =======================================================================
// c) FUNKCJA PolynomialReduce
// =======================================================================
template <typename Comparator>
tuple<vector<MultiPoly>, MultiPoly> PolynomialReduce(MultiPoly f, vector<MultiPoly> G, Comparator comp) {
    int n = f.num_vars;
    int s = G.size();
    
    vector<MultiPoly> a(s, MultiPoly(n)); 
    MultiPoly r(n); 
    MultiPoly p = f;
    
    p.normalize(comp);
    for (auto& g : G) g.normalize(comp);

    while (!p.is_zero()) {
        bool division_occurred = false;
        for (int i = 0; i < s; ++i) {
            if (G[i].is_zero()) continue;
            
            Term lt_p = p.leading_term();
            Term lt_g = G[i].leading_term();
            
            if (term_divides(lt_p, lt_g)) {
                Term q = divide_terms(lt_p, lt_g);
                
                a[i].add_term(q.coeff, q.exp);
                a[i].normalize(comp);
                
                MultiPoly subtractor = G[i].multiply_by_term(q);
                p = p - subtractor;
                p.normalize(comp);
                
                division_occurred = true;
                break; 
            }
        }
        
        if (!division_occurred) {
            Term lt_p = p.leading_term();
            r.add_term(lt_p.coeff, lt_p.exp);
            r.normalize(comp);
            
            MultiPoly lt_poly(n);
            lt_poly.add_term(lt_p.coeff, lt_p.exp);
            p = p - lt_poly;
            p.normalize(comp);
        }
    }
    
    return {a, r};
}

// =======================================================================
// TESTY I WYNIKI (d, e)
// =======================================================================
int main() {
    vector<string> vars = {"x", "y", "z"};
    
    cout << "--- d) Cwiczenie 37 ---\n";
    MultiPoly f(3);
    f.add_term(1.0, {3, 0, 0});  
    f.add_term(-1.0, {2, 1, 0}); 
    f.add_term(-1.0, {2, 0, 1}); 

    MultiPoly g1(3);
    g1.add_term(1.0, {2, 1, 0}); 
    g1.add_term(-1.0, {0, 0, 1});
    
    MultiPoly g2(3);
    g2.add_term(1.0, {1, 1, 0}); 
    g2.add_term(-1.0, {0, 0, 0});
    
    vector<MultiPoly> G1 = {g1, g2};
    vector<MultiPoly> G2 = {g2, g1}; 

    auto [alpha1, r1] = PolynomialReduce(f, G1, GrLex());
    cout << "Reszta r1 (g1, g2): "; r1.print(vars); cout << "\n";
    
    auto [alpha2, r2] = PolynomialReduce(f, G2, GrLex());
    cout << "Reszta r2 (g2, g1): "; r2.print(vars); cout << "\n";

    // -------------------------------------------------------------
    // INTERAKTYWNA CZĘŚĆ (Podpunkt e)
    // -------------------------------------------------------------
    cout << "\n--- e) Redukcja z 3 roznymi porzadkami ---\n";
    
    string indeks;
    cout << "Podaj swoj 6-cyfrowy numer indeksu (np. 186021): ";
    cin >> indeks;

    if(indeks.length() != 6) {
        cout << "Blad! Numer indeksu musi skladac sie dokladnie z 6 cyfr.\n";
        return 1;
    }

    int a = indeks[0] - '0';
    int b = indeks[1] - '0';
    int c = indeks[2] - '0';
    int d = indeks[3] - '0';
    int e = indeks[4] - '0';
    int f_val = indeks[5] - '0'; 

    cout << "\nPobrane zmienne: \n";
    cout << "a=" << a << ", b=" << b << ", c=" << c 
         << ", d=" << d << ", e=" << e << ", f=" << f_val << "\n\n";

    MultiPoly h(3);
    h.add_term(1.0, {a, b, 0});
    h.add_term(-1.0, {0, c, d});
    h.add_term(1.0, {e, 0, f_val});

    MultiPoly p1(3); p1.add_term(1.0, {1, 1, 0}); p1.add_term(-1.0, {0, 0, 2}); 
    MultiPoly p2(3); p2.add_term(1.0, {0, 2, 0}); p2.add_term(-1.0, {1, 0, 0}); 
    vector<MultiPoly> G_test = {p1, p2};

    cout << "Twój Wielomian H: "; h.normalize(Lex()); h.print(vars); cout << "\n\n";

    // 1. Lex
    auto [a_lex, res_lex] = PolynomialReduce(h, G_test, Lex());
    cout << "Reszta (LEX): "; res_lex.print(vars); cout << "\n";

    // 2. Permuted Lex (z > y > x)
    auto [a_plex, res_plex] = PolynomialReduce(h, G_test, PermutedLex({2, 1, 0}));
    cout << "Reszta (PermutedLEX z>y>x): "; res_plex.print(vars); cout << "\n";

    // 3. Graded Lex
    auto [a_grlex, res_grlex] = PolynomialReduce(h, G_test, GrLex());
    cout << "Reszta (GrLex): "; res_grlex.print(vars); cout << "\n";

    return 0;
}