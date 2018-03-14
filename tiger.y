%{
#include <iostream>
#include "ast.hh"

int yylex();
void yyerror (char const *s) {
   std::cerr << s << std::endl;
}

std::shared_ptr<tiger::ASTNode> programNode;

using namespace tiger;


%}

%union {
  tiger::ASTNode::ASTptr node;
}

%token TYPE NEW ARRAY OF VAR ASSIGNMENT FUNCTION
%token ENDL ENDF
%token NIL INTLIT STRINGLIT ID
%type<node> program exp exp_

/* precedence rules */
%left '|'
%left '&'
%left '<' '>' '=' "<>" "<=" ">="
%left '+' '-'
%left '*' '/'

%%
program: exp ENDF         {programNode = std::shared_ptr<ParentASTNode>(new ParentASTNode("program", {$1}));}
 | decs ENDF              /*{$$ = new ParentASTNode("program", {$1});}*/
;

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

/* exps separated by semicolons */
exps: 
 | exps_

/* can't be empty string */
exps_: exp
 | exp ';' exps_

/* helper for array creation and indexing */
arry: ID '[' exp ']'

exp: exp_                       {$$ = new ParentASTNode("exp", {$1});}
 | arry OF exp_

exp_: NIL                       {$$ = new TokenASTNode(NIL, "nil"); }
 | INTLIT                       {}
 | STRINGLIT
/* array and record creation */
 | typeId '{' '}'
 | typeId '{' recs '}'
/* Objects creation */
 | NEW typeId
/* Variables, filed, elements of an array */
 | lvalue
/* function call */
 | ID '(' ')'
 | ID '(' explist ')'
/* method call */
 | lvalue '.' ID '(' ')'
 | lvalue '.' ID '(' explist ')'
/* Operations */
 | '-' exp_
 | exp_ '+' exp_
 | exp_ '-' exp_
 | exp_ '*' exp_
 | exp_ '/' exp_
 | exp_ '=' exp_
 | exp_ "<>" exp_
 | exp_ '>' exp_
 | exp_ '<' exp_
 | exp_ ">=" exp_
 | exp_ "<=" exp_
 | exp_ '&' exp_
 | exp_ '|' exp_
 | '(' exps ')'
;

lvalue: ID
 | lvalue '.' ID
 | arry
 | lvalue '.' arry
;
%%
