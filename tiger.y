%{
#include <stdio.h>
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
void yyerror (char const *s) {
   fprintf (stderr, "%s\n", s);
}
