%option main
%%
\"([^"\n]|\"\")*\"      ECHO;
'.'                     ECHO;
[ \t]*--.*              ;
