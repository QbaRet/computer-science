
%option noyywrap

%{
#include <stdio.h>

int lines = 0;         
int words = 0;         
int pending_space = 0;  
int line_open = 0;      

void print_word(void);
void print_newline(void);
%}


BLANK   [ \t\r\f\v]
WORD    [^ \t\r\f\v\n]+

%%

^{BLANK}*\n     { /* pusta linia     -> usuń */ }
^{BLANK}+       { /* wcięcie         -> usuń */ }
{BLANK}*\n      { print_newline();  /* spacje na końcu + enter -> sam enter */ }
{BLANK}+        { pending_space = 1; /* spacje w środku -> jedna spacja */ }
{WORD}          { print_word(); }

%%

void print_word(void) {
    if (pending_space)
        putchar(' ');
    pending_space = 0;

    ECHO;
    words++;
    line_open = 1;
}

void print_newline(void) {
    putchar('\n');
    lines++;
    pending_space = 0;
    line_open = 0;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) { perror(argv[1]); return 1; }
    }

    yylex();

    if (line_open)             
        print_newline();

    printf("linie: %d\n", lines);
    printf("slowa: %d\n", words);
    return 0;
}
