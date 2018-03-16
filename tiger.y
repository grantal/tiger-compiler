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

%type<node> program id exp exps lValue decs dec ty classFields

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

/*Since id shows up so many places, we want it to be a node and not a string*/
id: ID                         {$$ = new TokenASTNode(ID, $1);}


exp: NIL                       {$$ = new TokenASTNode(NIL, "nil"); }
 | INTLIT                      {$$ = new TokenASTNode(INTLIT, $1);}
 | STRINGLIT
/* array and record creation */
 | id '['exp']' OF exp         {}
 | id '{' '}'
 | id '{' recs '}'
/* Objects creation */
 | NEW id
/* Variables, filed, elements of an array */
 | lValue
 | id
/* function call */
 | id '(' ')'
 | id '(' expList ')'
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
 | id ASSIGNMENT exp                {$$ = new ParentASTNode("assignment",nodeType::ASSIGNMENT_, {$1, $3});}
/*Control structures */
 | IF exp THEN exp                  {$$ = new ParentASTNode("if then",nodeType::IF_THEN, {$2, $4});}
 | IF exp THEN exp ELSE exp         {$$ = new ParentASTNode("if then else",nodeType::IF_THEN, {$2, $4, $6});}
 | WHILE exp DO exp                 {$$ = new ParentASTNode("while do",nodeType::WHILE_DO, {$2, $4});}
 | FOR id ASSIGNMENT exp TO exp DO exp {$$ = new ParentASTNode("for to do",nodeType::FOR_TO_DO, {$2, $4, $6, $8});}
 | BREAK                            {$$ = new TokenASTNode(BREAK, "break");}
 | LET IN END                       {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {nullptr, nullptr});}  
 | LET IN exps END                  {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {nullptr, $3});}
 | LET decs IN END                  {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {$2, nullptr});}
 | LET decs IN exps END             {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {$2, $4});}
;

/*=========HELPERS FOR EXP=============*/
recs
 : id '=' exp
 | id '=' exp ',' recs
;
/* exps separated by commas */
expList
 : exp
 | exp ',' expList
;

/* Departure: I cover the single id case above and the id > 1 case here */
lValue 
 : id '.' id
 | lValue '.' id
 | lValue '[' exp ']'
 | id '[' exp ']'
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
 : TYPE id '=' ty                           {$$ = new ParentASTNode("type declation", nodeType::TYPE_DEC, {$2, $4});}
/*class definition NOTE: the id of what it extends is the third child*/
 | CLASS id '{' classFields '}'             {$$ = new ParentASTNode("class declaration", nodeType::CLASS_DEC, {$2, $4});}
 | CLASS id EXTENDS id '{' classFields '}'  {$$ = new ParentASTNode("class declaration w/ extends", nodeType::CLASS_DEC, {$2, $6, $4});}

/*variable declaration */
 | varDec
/*function declartation */
 | FUNCTION id '(' tyFields ')' '=' exp
 | FUNCTION id '(' tyFields ')' ':' id '=' exp
/*primitive declaration */
 | PRIMITIVE id '(' tyFields ')'
 | PRIMITIVE id '(' tyFields ')' ':' id
/*importing declarations*/
 | IMPORT STRINGLIT
; 

/*helper rules for decs */

varDec
 : VAR id ASSIGNMENT exp
 | VAR id ':' id ASSIGNMENT exp

/*Departure: I cover the classfields = 0 case whereever classFields is used */
classFields
 : classField
 | classField classFields
;

classField
 : varDec
 | METHOD id '(' tyFields ')' '=' exp
 | METHOD id '(' ')' '=' exp
 | METHOD id '(' tyFields ')' ':' id '=' exp
 | METHOD id '(' ')' ':' id '=' exp

ty
 : id
 /*Record type definition.*/
 | '{' '}'
 | '{' tyFields '}'
 /*Array type definition. */
 | ARRAY OF id
 /*Class definition (canonical form). */
 | CLASS '{' '}'
 | CLASS '{' classFields '}'
 | CLASS EXTENDS id '{' '}'
 | CLASS EXTENDS id '{' classFields '}'
;

/*Departure: I coever the tyFields = 0 case wherever tyFields
 is used. Further, type-id has been eleminated and 
 replaced with id where type-id was used since the conflicts
 that arose were due to lack of context.
 */
tyFields
 : id ':' id
 | id ':' id ',' tyFields
;


%%
