%{
#include <stdio.h>

int yylex();
void yyerror (char const *s) {
   fprintf (stderr, "%s\n", s);
}
%}
%token ENDL VAR

%%
decs: 
 | dec
 | dec ENDL decs
;

dec: VAR
; 
%%
