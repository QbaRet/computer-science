/* Zadanie 3: usuwanie komentarzy C++; z opcja -d zostawia komentarze
   dokumentacyjne Doxygena: / ** , / *! , ///  i  //!                   */
%option noyywrap
%{
#include <stdio.h>
#include <string.h>
int keep_doc = 0;
char rawend[20];  int rawlen, rawpos;   /* koniec raw stringa: )delim" */
%}
%x COMMENT DOC RAW
LC   ([^\\\n]|\\(.|\n))*
%%
\"([^"\\\n]|\\(.|\n))*\"          ECHO;   /* "napis"            */
'([^'\\\n]|\\.)+'                 ECHO;   /* 'z'                */
[A-Za-z_][A-Za-z0-9_]*            ECHO;   /* identyfikator      */
[0-9]([0-9A-Za-z_.]|'[0-9A-Za-z_])* ECHO; /* liczba, np. 1'000  */
(u8|u|U|L)?R\"[^ ()\\\t\n"]{0,16}\( {      /* R"delim( ... )delim" */
        ECHO;
        char *q = strchr(yytext, '"') + 1;
        rawlen = sprintf(rawend, ")%.*s\"", (int)(yyleng - (q - yytext) - 1), q);
        rawpos = 0; BEGIN(RAW); }
<RAW>.|\n  { ECHO;
             if (yytext[0] == rawend[rawpos]) { if (++rawpos == rawlen) BEGIN(INITIAL); }
             else rawpos = (yytext[0] == ')'); }

"/**/"                  putchar(' ');               /* pusty komentarz   */
"/**"|"/*!"             { if (keep_doc) { ECHO; BEGIN(DOC); } else BEGIN(COMMENT); }
"/*"                    BEGIN(COMMENT);
<COMMENT>"*/"           { putchar(' '); BEGIN(INITIAL); }
<COMMENT>.|\n           ;
<DOC>"*/"               { ECHO; BEGIN(INITIAL); }
<DOC>.|\n               ECHO;

"////"{LC}              ;                           /* //// to nie doc   */
"///"{LC}|"//!"{LC}     { if (keep_doc) ECHO; }
"//"{LC}                ;
%%
int main(int argc, char **argv) {
    int i = 1;
    if (i < argc && strcmp(argv[i], "-d") == 0) { keep_doc = 1; i++; }
    if (i < argc && !(yyin = fopen(argv[i], "r"))) { perror(argv[i]); return 1; }
    yylex();
    return 0;
}
