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

%type<node> program id typeId exp exps lValue decs dec ty tyFields classFields classField varDec

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
;

typeId: id                     {$$ = new ParentASTNode("type id",nodeType::TYPE_ID,{$1});} 
;

exp: NIL                       {$$ = new TokenASTNode(NIL, "nil"); }
 | INTLIT                      {$$ = new TokenASTNode(INTLIT, $1);}
 | STRINGLIT
/* array and record creation */
 | id '['exp']' OF exp         {$$ = new ParentASTNode("Array", nodeType::ARRAY, {new ParentASTNode("type id",nodeType::TYPE_ID,{$1}), $3, $6});}
 | typeId '{' '}'
 | typeId '{' recs '}'
/* Objects creation */
 | NEW typeId
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
 | CLASS id EXTENDS typeId '{' classFields '}'  {$$ = new ParentASTNode("class declaration w/ extends", nodeType::CLASS_DEC, {$2, $6, $4});}

/*variable declaration */
 | varDec                                   {$$ = $1;}
/*function declartation */
 | FUNCTION id '(' tyFields ')' '=' exp     {$$ = new ParentASTNode("function declaration", nodeType::FUNC_DEC, {$2, $4, $7});}
 | FUNCTION id '(' tyFields ')' ':' typeId '=' exp {$$ = new ParentASTNode("function declaration", nodeType::FUNC_DEC, {$2, $4, $9, $7});}

/*primitive declaration */
 | PRIMITIVE id '(' tyFields ')'            {$$ = new ParentASTNode("primitive declaration", nodeType::PRIM_DEC, {$2, $4});}

 | PRIMITIVE id '(' tyFields ')' ':' typeId {$$ = new ParentASTNode("primitive declaration", nodeType::PRIM_DEC, {$2, $4, $7});}

/*importing declarations*/
 | IMPORT STRINGLIT                         {$$ = new ParentASTNode("import", nodeType::IMPORT_, {new TokenASTNode(STRINGLIT, $2)});}

; 

/*helper rules for decs */

varDec
 : VAR id ASSIGNMENT exp                    {$$ = new ParentASTNode("variable declaration", nodeType::VAR_DEC, {$2, $4});}
 | VAR id ':' typeId ASSIGNMENT exp         {$$ = new ParentASTNode("variable declaration", nodeType::VAR_DEC, {$2, $6, $4});}


/*Departure: I cover the classfields = 0 case whereever classFields is used */
classFields
 : classField                               {$$ = new ParentASTNode("class fields", nodeType::CLASS_FIELDS, {$1});}

 | classField classFields                   {$$ = new ParentASTNode("class fields", nodeType::CLASS_FIELDS, {$1, $2});}
;

classField
 : varDec                                   {$$ = new ParentASTNode("class field", nodeType::CLASS_FIELD, {$1});}
 | METHOD id '(' tyFields ')' '=' exp       {$$ = new ParentASTNode("class field Method Declaration", nodeType::CLASS_FIELD, {$2, $4, nullptr, $7});}

 | METHOD id '(' ')' '=' exp                {$$ = new ParentASTNode("class field Method Declaration", nodeType::CLASS_FIELD, {$2, nullptr, nullptr, $6});}
 | METHOD id '(' tyFields ')' ':' typeId '=' exp {$$ = new ParentASTNode("class field Method Declaration", nodeType::CLASS_FIELD, {$2, $4, $7, $9});}
 | METHOD id '(' ')' ':' typeId '=' exp     {$$ = new ParentASTNode("class field Method Declaration", nodeType::CLASS_FIELD, {$2, nullptr, $6, $8});}

ty
 : typeId                                   {$$ = $1;}
 /*Record type definition.*/
 | '{' '}'                                  {$$ = new ParentASTNode("record type definition", nodeType::REC_TYPE, {});}
 | '{' tyFields '}'                         {$$ = new ParentASTNode("record type definition", nodeType::REC_TYPE, {$2});}
 /*Array type definition. */
 | ARRAY OF typeId                          {$$ = new ParentASTNode("array type definition", nodeType::ARRAY_TYPE, {$3});}
 /*Class definition (canonical form). */
 | CLASS '{' '}'                            {$$ = new ParentASTNode("class definition", nodeType::CLASS_TYPE, {});}
 | CLASS '{' classFields '}'                {$$ = new ParentASTNode("class definition", nodeType::CLASS_TYPE, {$3});}
 | CLASS EXTENDS typeId '{' '}'             {$$ = new ParentASTNode("class definition", nodeType::CLASS_TYPE, {nullptr, $3});}
 | CLASS EXTENDS typeId '{' classFields '}' {$$ = new ParentASTNode("class definition", nodeType::CLASS_TYPE, {$5, $3});}
;

/*Departure: I coever the tyFields = 0 case wherever tyFields
 is used. 
 */
tyFields
 : id ':' typeId                           {$$ = new ParentASTNode("type fields", nodeType::TY_FIELDS, {$1, $3});}

 | id ':' typeId ',' tyFields              {$$ = new ParentASTNode("type fields", nodeType::TY_FIELDS, {$1, $3, $5});}
;


%%
