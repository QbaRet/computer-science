/** Doc blokowy */
/*! Doc Qt */
/* zwykly */
/**/
#include <iostream>
/// doc liniowy
//! doc liniowy 2
//// nie doc
// zwykly \
   kontynuacja komentarza
int main() {
    const char *s = "nie /* komentarz */ // tez nie \" ";
    char c = '"'; int x = 1'000; // it's
    auto r = R"xy(raw // nie komentarz )" /* */ )xy";
    int y = 2/**/+3; /* wielo
       liniowy */ return 0; // koniec
}
