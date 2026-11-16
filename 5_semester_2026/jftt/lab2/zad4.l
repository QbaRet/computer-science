%option main
%{
#include <limits.h>
int stack[1000], top = 0;
void number(char *text);
void calc(char op);
void error(char *msg);
void end_line(void);
%}
%x ERR
%%
-?[0-9]+                number(yytext);
[-+*/^%]                calc(yytext[0]);
[ \t]+                  ;
\n                      end_line();
[^-+*/^% \t\n0-9]+      { printf("Błąd: zły symbol \"%s\"\n", yytext); top = 0; BEGIN(ERR); }
<ERR>.*                 ;
<ERR>\n                 BEGIN(INITIAL);
<<EOF>>                 { end_line(); yyterminate(); }
%%
int out_of_range(long long x) {
    return x < INT_MIN || x > INT_MAX;
}

void error(char *msg) {
    printf("Błąd: %s\n", msg);
    top = 0;
    BEGIN(ERR);
}

void number(char *text) {
    long long x = atoll(text);
    if (out_of_range(x)) error("liczba poza zakresem");
    else stack[top++] = x;
}

void calc(char op) {
    if (top < 2) { error("za mała liczba argumentów"); return; }
    long long b = stack[--top];
    long long a = stack[--top];
    if ((op == '/' || op == '%') && b == 0) { error("dzielenie przez zero"); return; }
    if (op == '^' && b < 0) { error("ujemny wykładnik"); return; }

    long long r = 1;
    switch (op) {
        case '+': r = a + b; break;
        case '-': r = a - b; break;
        case '*': r = a * b; break;
        case '/': r = a / b; break;
        case '%': r = a % b; break;
        case '^': while (b-- > 0 && !out_of_range(r)) r *= a; break;
    }
    if (out_of_range(r)) error("wynik poza zakresem");
    else stack[top++] = r;
}

void end_line(void) {
    if (top == 1) printf("= %d\n", stack[0]);
    else if (top > 1) printf("Błąd: za mała liczba operatorów\n");
    top = 0;
}
