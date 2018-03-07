%{
#include <iostream>

int yylex();
void yyerror (char const *s) {
   std::cerr << s << std::endl;
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
