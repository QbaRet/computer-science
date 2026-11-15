/* Zadanie 2: usuwanie komentarzy z plikow Ady (-- do konca linii) */
%option noyywrap
%%
\"([^"\n]|\"\")*\"   ECHO;   /* literal napisowy, "" = cudzyslow w srodku */
'.'                  ECHO;   /* literal znakowy, np. '-' lub '"'          */
[ \t]*--.*           ;       /* komentarz (wraz ze spacjami przed nim)    */
%%
int main(int argc, char **argv) {
    if (argc > 1 && !(yyin = fopen(argv[1], "r"))) { perror(argv[1]); return 1; }
    yylex();
    return 0;
}
