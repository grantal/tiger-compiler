%{
#include <iostream>

int yylex();
void yyerror (char const *s) {
   std::cerr << s << std::endl;
}
%}
%token ENDL TYPE ID ARRAY OF VAR ASSIGNMENT FUNCTION
%token NIL INTLIT STRINGLIT

%%
decs: 
 | dec
 | dec ENDL decs
;

dec: tydec
 | vardec
 | fundec
; 

tydec: TYPE typeid '=' ty
;

typeid: ID
;

ty: typeid
 | '{' tyfields '}'
 | ARRAY OF typeid
;

tyfields:
 | ID ':' typeid
 | ID ':' typeid ',' tyfields
;

vardec: VAR ID ASSIGNMENT exp
 | VAR ID ':' typeid ASSIGNMENT exp
;

fundec: FUNCTION ID '(' tyfields ')' '=' exp
 | FUNCTION ID '(' tyfields ')' ':' typeid '=' exp
;

exp: NIL
 | INTLIT
 | STRINGLIT
 | typeid '[' exp ']' OF exp
 | typeid '{' ID '=' exp
%%
