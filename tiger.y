%{
#include <iostream>
int yylex();
void yyerror (char const *s) {
   std::cerr << s << std::endl;
}
%}

%token ENDL TYPE ID ARRAY OF VAR ASSIGNMENT FUNCTION
%token NIL INTLIT STRINGLIT NEW
%token IF THEN ELSE WHILE FOR TO DO LET IN END BREAK
%token CLASS EXTENDS PRIMITIVE IMPORT METHOD

/* precedence rules (Added Assingnment op) */
%left op
%left ASSIGNMENT
%left '|'
%left '&'
%left '<' '>' '=' "<>" "<=" ">="
%left '+' '-'
%left '*' '/'

/*Departure Bison and Flex suggested nonassoc to fix 
control flow shift/reduce conflicts */
%nonassoc THEN
%nonassoc ELSE
%nonassoc DO
%nonassoc OF

%%



program
 : exp
 | decs

exp
 : NIL
 | INTLIT
 | STRINGLIT
/* array and record creation */
 | ID '['exp']' OF exp
 | ID '{' '}'
 | ID '{' recs '}'
/* Objects creation */
 | NEW ID
/* Variables, filed, elements of an array */
 | lValue
 | ID
/* function call */
 | ID '(' ')'
 | ID '(' expList ')'
/* method call */
 | lValue '(' ')'
 | lValue '(' expList ')'
/* Operations */
 | '-' exp
 | exp '+' exp
 | exp '-' exp
 | exp '*' exp
 | exp '/' exp
 | exp '=' exp
 | exp "<>" exp
 | exp '>' exp
 | exp '<' exp
 | exp ">=" exp
 | exp "<=" exp
 | exp '&' exp
 | exp '|' exp
 | '(' exps ')'
/*Assignment */
 | lValue ASSIGNMENT exp
 | ID ASSIGNMENT exp
/*Control structures */
 | IF exp THEN exp
 | IF exp THEN exp ELSE exp
 | WHILE exp DO exp
 | FOR ID ASSIGNMENT TO exp DO exp
 | BREAK
 | LET IN exps END
 | LET decs IN exps END
;
/*=========HELPERS FOR EXP=============*/
recs
 : ID '=' exp
 | ID '=' exp ',' recs
;
/* exps separated by commas */
expList
 : exp
 | exp ',' expList
;

/* Departure: I cover the single ID case above and the ID > 1 case here */
lValue 
 : ID '.' ID
 | lValue '.' ID
 | lValue '[' exp ']'
 | ID '[' exp ']'
;

/* exps separated by semicolons */
exps
 : exp
 | exp ';' exps
 ;

/*Departure: I cover the decs = 0 case wherever decs is used */
decs
 : dec
 | dec  decs
;

dec
 : TYPE ID '=' ty
/*class definition */
 | CLASS ID '{' classFields '}'
 | CLASS ID EXTENDS ID '{' classFields '}'
/*variable declaration */
 | varDec
/*function declartation */
 | FUNCTION ID '(' tyFields ')' '=' exp
 | FUNCTION ID '(' tyFields ')' ':' ID '=' exp
/*primitive declaration */
 | PRIMITIVE ID '(' tyFields ')'
 | PRIMITIVE ID '(' tyFields ')' ':' ID
/*importing declarations*/
 | IMPORT STRINGLIT
; 

/*helper rules for decs */

varDec
 : VAR ID ASSIGNMENT exp
 | VAR ID ':' ID ASSIGNMENT exp

/*Departure: I cover the classfields = 0 case whereever classFields is used */
classFields
 : classField
 | classField classFields
;

classField
 : varDec
 | METHOD ID '(' tyFields ')' '=' exp
 | METHOD ID '(' ')' '=' exp
 | METHOD ID '(' tyFields ')' ':' ID '=' exp
 | METHOD ID '(' ')' ':' ID '=' exp

ty
 : ID
 /*Record type definition.*/
 | '{' '}'
 | '{' tyFields '}'
 /*Array type definition. */
 | ARRAY OF ID
 /*Class definition (canonical form). */
 | CLASS '{' '}'
 | CLASS '{' classFields '}'
 | CLASS EXTENDS ID '{' '}'
 | CLASS EXTENDS ID '{' classFields '}'
;

/*Departure: I coever the tyFields = 0 case wherever tyFields
 is used. Further, type-id has been eleminated and 
 replaced with ID where type-ID was used since the conflicts
 that arose were due to lack of context.
 */
tyFields
 : ID ':' ID
 | ID ':' ID ',' tyFields
;


%%