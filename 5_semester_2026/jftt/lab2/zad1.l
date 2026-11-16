%option main
%{
int lines = 0, words = 0;
int space = 0;  /* czy przed następnym słowem wstawić spację */
int open = 0;   /* czy ostatnia linia nie ma jeszcze entera */
%}
%%
^[ \t]*\n       ;
^[ \t]+         ;
[ \t]*\n        { putchar('\n'); lines++; open = 0; }
[ \t]+          space = 1;
[^ \t\n]+       { if (space) putchar(' '); space = 0; ECHO; words++; open = 1; }
<<EOF>>         {
                    if (open) { putchar('\n'); lines++; }
                    printf("linie: %d\nslowa: %d\n", lines, words);
                    yyterminate();
                }
