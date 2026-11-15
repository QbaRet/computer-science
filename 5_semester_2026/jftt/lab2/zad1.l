/* Zadanie 1: normalizacja bialych znakow + liczba linii i slow */
%option noyywrap
%{
#include <stdio.h>
int lines = 0, words = 0;
int need_nl = 0;          /* ostatnia linia nie konczyla sie '\n' */
int space = 0;            /* zalegla spacja do wypisania przed slowem */
%}
WS  [ \t\r\f\v]
%%
^{WS}*\n        ;                                    /* pusta linia      */
^{WS}+          ;                                    /* poczatek wiersza */
{WS}+\n         { putchar('\n'); lines++; need_nl = space = 0; } /* koniec wiersza */
{WS}+           { space = 1; }                       /* ciag -> 1 spacja */
\n              { putchar('\n'); lines++; need_nl = space = 0; }
[^ \t\r\f\v\n]+ { if (space) putchar(' '); space = 0; ECHO; words++; need_nl = 1; }
%%
int main(int argc, char **argv) {
    if (argc > 1 && !(yyin = fopen(argv[1], "r"))) { perror(argv[1]); return 1; }
    yylex();
    if (need_nl) { putchar('\n'); lines++; }
    printf("linie: %d\nslowa: %d\n", lines, words);
    return 0;
}
