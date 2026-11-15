/* Zadanie 4: kalkulator w odwrotnej notacji polskiej */
%option noyywrap
%{
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#define MAX 1024
long stack[MAX]; int top = 0;
void push(long x);
void calc(char op);
void error(const char *msg);
void end_line(void);
%}
%x ERR
%%
-?[0-9]+          { errno = 0; long x = strtol(yytext, NULL, 10);
                    if (errno) error("liczba poza zakresem"); else push(x); }
[-+*/^%]          calc(yytext[0]);
[ \t]+            ;
\n                end_line();
[^-+*/^% \t\n0-9]+ { printf("Błąd: zły symbol \"%s\"\n", yytext); top = 0; BEGIN(ERR); }
<ERR>.*           ;                      /* pomin reszte blednej linii */
<ERR>\n           BEGIN(INITIAL);
<<EOF>>           { end_line(); yyterminate(); }  /* ostatnia linia bez entera */
%%
void error(const char *msg) {            /* blad: zadnych wynikow, pomin linie */
    printf("Błąd: %s\n", msg);
    top = 0;
    BEGIN(ERR);
}

void push(long x) {
    if (top == MAX) error("przepełnienie stosu");
    else stack[top++] = x;
}

void calc(char op) {
    if (top < 2) { error("za mała liczba argumentów"); return; }
    long b = stack[--top], a = stack[--top], r = 1;
    int ovf = 0;
    switch (op) {
      case '+': ovf = __builtin_add_overflow(a, b, &r); break;
      case '-': ovf = __builtin_sub_overflow(a, b, &r); break;
      case '*': ovf = __builtin_mul_overflow(a, b, &r); break;
      case '/':
      case '%': if (b == 0) { error("dzielenie przez zero"); return; }
                if (a == LONG_MIN && b == -1) ovf = 1;
                else r = (op == '/') ? a / b : a % b;
                break;
      case '^': if (b < 0) { error("ujemny wykładnik potęgi"); return; }
                while (b > 0 && !ovf) {          /* szybkie potegowanie */
                    if (b & 1) ovf = __builtin_mul_overflow(r, a, &r);
                    b >>= 1;
                    if (b) ovf |= __builtin_mul_overflow(a, a, &a);
                }
                break;
    }
    if (ovf) error("przekroczenie zakresu liczb");
    else push(r);
}

void end_line(void) {
    if (top == 1)     printf("= %ld\n", stack[0]);
    else if (top > 1) printf("Błąd: za mała liczba operatorów\n");
    top = 0;
}

int main(void) { yylex(); return 0; }
