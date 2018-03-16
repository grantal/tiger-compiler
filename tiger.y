%{
#include <iostream>
#include "ast.hh"
#include <memory>

int yylex();
void yyerror (char const *s) {
   std::cerr << s << std::endl;
}

namespace tiger {
/* global variable to store the result of the parser */
std::shared_ptr<ASTNode> programNode;
}

using namespace tiger;


%}

%union {
  tiger::ASTNode::ASTptr node;
  char *strVal;
}

%type<node> program exp exps lValue decs dec ty classFields

%token<strVal> INTLIT STRINGLIT ID
%token ENDL NIL NEW TYPE ARRAY OF VAR ASSIGNMENT FUNCTION
%token IF THEN ELSE WHILE FOR TO DO LET IN END BREAK
%token CLASS EXTENDS PRIMITIVE IMPORT METHOD
%token ERROR NOTEQUAL ELESS EGREATER

/* precedence rules (Added Assingnment op) */
%left op
%left ASSIGNMENT
%left '|'
%left '&'
%left '<' '>' '=' NOTEQUAL ELESS EGREATER
%left '+' '-'
%left '*' '/'

/*Departure Bison and Flex suggested nonassoc to fix 
control flow shift/reduce conflicts */
%nonassoc THEN
%nonassoc ELSE
%nonassoc DO
%nonassoc OF

%%
program: exp         {programNode = std::shared_ptr<ParentASTNode>(new ParentASTNode("program",nodeType::PROGRAM, {$1}));}
 | decs               
;

exp: NIL                       {$$ = new TokenASTNode(NIL, "nil"); }
 | INTLIT                      {$$ = new TokenASTNode(INTLIT, $1);}
 | STRINGLIT
/* array and record creation */
 | ID '['exp']' OF exp         {}
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
 | exp NOTEQUAL exp
 | exp '>' exp
 | exp '<' exp
 | exp EGREATER exp
 | exp ELESS exp
 | exp '&' exp
 | exp '|' exp                      {$$ = new ParentASTNode("or",nodeType::OR, {$1, $3});}
 | '(' exps ')'                     {$$ = new ParentASTNode("sequence",nodeType::SEQUENCE, {$2});}
/*Assignment */
 | lValue ASSIGNMENT exp            {$$ = new ParentASTNode("assignment",nodeType::ASSIGNMENT_, {$1, $3});}
 | ID ASSIGNMENT exp                {$$ = new ParentASTNode("assignment",nodeType::ASSIGNMENT_, {$1, $3});}
/*Control structures */
 | IF exp THEN exp                  {$$ = new ParentASTNode("if then",nodeType::IF_THEN, {$2, $4});}
 | IF exp THEN exp ELSE exp         {$$ = new ParentASTNode("if then else",nodeType::IF_THEN, {$2, $4, $6});}
 | WHILE exp DO exp                 {$$ = new ParentASTNode("while do",nodeType::WHILE_DO, {$2, $4});}
 | FOR ID ASSIGNMENT exp TO exp DO exp {$$ = new ParentASTNode("for to do",nodeType::FOR_TO_DO, {$2, $4, $6, $8});}
 | BREAK                            {$$ = new TokenASTNode(BREAK, "break");}
 | LET IN END                       {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {nullptr, nullptr});}  
 | LET IN exps END                  {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {nullptr, $3});}
 | LET decs IN END                  {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {$2, nullptr});}
 | LET decs IN exps END             {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {$2, $4});}
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
 : dec                                      {$$ = new ParentASTNode("decs", nodeType::DECS, {$1});} 
 | dec  decs                                {$$ = new ParentASTNode("decs", nodeType::DECS, {$1, $2});}
;

dec
 : TYPE ID '=' ty                           {$$ = new ParentASTNode("type declation", nodeType::TYPE_DEC, {$2, $4});}
/*class definition NOTE: the id of what it extends is the third child*/
 | CLASS ID '{' classFields '}'             {$$ = new ParentASTNode("class declaration", nodeType::CLASS_DEC, {$2, $4});}
 | CLASS ID EXTENDS ID '{' classFields '}'  {$$ = new ParentASTNode("class declaration w/ extends", nodeType::CLASS_DEC, {$2, $6, $4});}

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
