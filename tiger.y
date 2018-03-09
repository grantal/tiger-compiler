%{
#include <iostream>

int yylex();
void yyerror (char const *s) {
   std::cerr << s << std::endl;
}
%}
%token ENDL TYPE ID ARRAY OF VAR ASSIGNMENT FUNCTION
%token NIL INTLIT STRINGLIT NEW

%%
decs: 
 | dec
 | dec ENDL decs
;

dec: tydec
 | vardec
 | fundec
; 

tydec: TYPE typeId '=' ty
;

typeId: ID
;

ty: typeId
 | '{' tyfields '}'
 | ARRAY OF typeId
;

tyfields:
 | ID ':' typeId
 | ID ':' typeId ',' tyfields
;

vardec: VAR ID ASSIGNMENT exp
 | VAR ID ':' typeId ASSIGNMENT exp
;

fundec: FUNCTION ID '(' tyfields ')' '=' exp
 | FUNCTION ID '(' tyfields ')' ':' typeId '=' exp
;

recs: ID '=' exp
 | ID '=' exp ',' recs
;

/* exps separated by commas */
explist: exp
 | exp ',' explist
;

exp: NIL
 | INTLIT
 | STRINGLIT
/* array and record creation */
 | typeId '[' exp ']' OF exp
 | typeId '{' '}'
 | typeId '{' recs '}'
/* Objects creation */
 | NEW typeId
/* Variables, filed, elements of an array */
 | lvalue
/* function call */
 | ID '(' ')'
 | ID '(' explist ')'
;

lvalue: ID
 | lvalue '.' ID
 | lvalue '[' exp ']'
;
%%
