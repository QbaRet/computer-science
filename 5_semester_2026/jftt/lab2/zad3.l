%option noyywrap
%{
#include <string.h>
int doc = 0;  /* opcja -d: zostaw komentarze dokumentacyjne */
%}
%x COMMENT DOC
LINE    ([^\\\n]|\\(.|\n))*
%%
\"(\\.|[^"\\\n])*\"     ECHO;
'(\\.|[^'\\\n])+'       ECHO;
[A-Za-z_][A-Za-z0-9_]*  ECHO;
[0-9]([0-9A-Za-z_.]|'[0-9A-Za-z_])*  ECHO;

"/**/"                  putchar(' ');
"/**"|"/*!"             { if (doc) { ECHO; BEGIN(DOC); } else BEGIN(COMMENT); }
"/*"                    BEGIN(COMMENT);
<COMMENT>"*/"           { putchar(' '); BEGIN(INITIAL); }
<COMMENT>.|\n           ;
<DOC>"*/"               { ECHO; BEGIN(INITIAL); }
<DOC>.|\n               ECHO;

"///"{LINE}|"//!"{LINE} { if (doc) ECHO; }
"//"{LINE}              ;
%%
int main(int argc, char **argv) {
    doc = argc > 1 && strcmp(argv[1], "-d") == 0;
    yylex();
    return 0;
}
