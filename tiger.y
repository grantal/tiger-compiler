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
%type<node> program id typeId exp exps lValue decs dec ty tyFields classFields varDec
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
 | typeId '{' '}'              {$$ = new ParentASTNode("Record", nodeType::RECORD,{$1,nullptr});}
 | typeId '{' recs '}'         {$$ = new ParentASTNode("Record", nodeType::RECORD,{$1,$3});}
/* Objects creation */
 | NEW typeId                  {$$ = new ParentASTNode("Object", nodeType::OBJECT, {$2,nullptr});}
/* Variables, filed, elements of an array */
 | lValue                      {$$ = new ParentASTNode("Reference", nodeType::REFERENCE, {$1,nullptr});}
 | id                          {$$ = new ParentASTNode("Reference", nodeType::REFERENCE, {$1,nullptr});}
/* function call */
 | id '(' ')'                  {$$ = new ParentASTNode("Call function", nodeType::CALL_FUNC,{$1,nullptr});}
 | id '(' expList ')'          {$$ = new ParentASTNode("Call function", nodeType::CALL_FUNC,{$1,$3});}
/* method call */
 | lValue '(' ')'              {$$ = new ParentASTNode("Call Method", nodeType::CALL_METHOD,{$1,nullptr});}
 | lValue '(' expList ')'      {$$ = new ParentASTNode("Call Method", nodeType::CALL_METHOD,{$1,$3});}
/* Operations */
 | '-' exp                     {$$ = new ParentASTNode("Negate", nodeType::NEGATE,{$1,$3})}
 | exp '+' exp                 {$$ = new ParentASTNode("Add", nodeType::ADD,{$1,$3})}
 | exp '-' exp                 {$$ = new ParentASTNode("Subtract", nodeType::SUB,{$1,$3})}
 | exp '*' exp                 {$$ = new ParentASTNode("Mulitply", nodeType::MULT,{$1,$3})
 | exp '/' exp                 {$$ = new ParentASTNode("Divide", nodeType::DIV,{$1,$3})
 | exp '=' exp                 {$$ = new ParentASTNode("Equal", nodeType::EQUAL,{$1,$3})
 | exp NOTEQUAL exp            {$$ = new ParentASTNode("Not Equal", nodeType::NOT_EQUAL,{$1,$3})
 | exp '>' exp                 {$$ = new ParentASTNode("Greater", nodeType::GREATER,{$1,$3})
 | exp '<' exp                 {$$ = new ParentASTNode("Lesser", nodeType::LESSER,{$1,$3})
 | exp EGREATER exp            {$$ = new ParentASTNode("Equal or Greater", nodeType::EQ_GREATER,{$1,$3})
 | exp ELESS exp               {$$ = new ParentASTNode("Equal or Less", nodeType::EQ_LESS,{$1,$3})
 | exp '&' exp                 {$$ = new ParentASTNode("And", nodeType::AND,{$1,$3})
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
 : id '=' exp                    {$$ = new ParentASTNode("Record Value", nodeType::REC_VAL,{$1,$3,nullptr});}
 | id '=' exp ',' recs           {$$ = new ParentASTNode("Record Value", nodeType::REC_VAL,{$1,$3,$5});}
;
/* exps separated by commas */
expList
 : exp     
 | exp ',' expList
;
/* Departure: I cover the single id case above and the id > 1 case here */
lValue 
 : id '.' id                                {$$ = new ParentASTNode("Reference", nodeType::REFERENCE,{$1,new ParentASTNode("Reference", nodeType::REFERENCE,{$3,nullptr})});}
 | lValue '.' id                            {$$ = new ParentASTNode("Referemce", nodeType::REFERENCE,{$1,new ParentASTNode("Reference", nodeType::REFERENCE,{$3,nullptr})})
 | lValue '[' exp ']'                       {$$ = new ParentASTNode("Reference", nodeType::REFERENCE,{$1,new ParentASTNode("Array Reference", nodeType::ARRAY_REF,{$1,$3,nullptr})})}
 | id '[' exp ']'                           {$$ = new ParentASTNode("Reference", nodeType::REFERENCE,{$1,new ParentASTNode("Array Reference", nodeType::ARRAY_REF,{$1,$3,nullptr})})}
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
 | FUNCTION id '(' tyFields ')' '=' exp     {$$ = new ParentASTNode("class declaration", nodeType::CLASS_DEC, {$2, $4});}
 | FUNCTION id '(' tyFields ')' ':' typeId '=' exp
/*primitive declaration */
 | PRIMITIVE id '(' tyFields ')'
 | PRIMITIVE id '(' tyFields ')' ':' typeId
/*importing declarations*/
 | IMPORT STRINGLIT
; 
/*helper rules for decs */
varDec
 : VAR id ASSIGNMENT exp
 | VAR id ':' typeId ASSIGNMENT exp
/*Departure: I cover the classfields = 0 case whereever classFields is used */
classFields
 : classField
 | classField classFields
;
classField
 : varDec
 | METHOD id '(' tyFields ')' '=' exp
 | METHOD id '(' ')' '=' exp
 | METHOD id '(' tyFields ')' ':' typeId '=' exp
 | METHOD id '(' ')' ':' typeId '=' exp
ty
 : typeId
 /*Record type definition.*/
 | '{' '}'
 | '{' tyFields '}'
 /*Array type definition. */
 | ARRAY OF typeId
 /*Class definition (canonical form). */
 | CLASS '{' '}'
 | CLASS '{' classFields '}'
 | CLASS EXTENDS typeId '{' '}'
 | CLASS EXTENDS typeId '{' classFields '}'
;
/*Departure: I coever the tyFields = 0 case wherever tyFields
 is used. Further, type-id has been eleminated and 
 replaced with id where type-id was used since the conflicts
 that arose were due to lack of context.
 */
tyFields
 : id ':' typeId
 | id ':' typeId ',' tyFields
;