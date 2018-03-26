%{
#include <iostream>
#include <memory>
#include "ast.hh"

int yylex();
extern int yylineno;
void yyerror (char const *s);
namespace tiger {
/* global variable to store the result of the parser */
std::shared_ptr<ParentASTNode> programNode;
}
using namespace tiger;
%}
%union {
  tiger::ASTNode::ASTptr node;
  char *strVal;
}

%locations

%type<node> id typeId exp opExp exps expList lValue decs dec ty tyFields classFields classField varDec recs

%token<strVal> INTLIT STRINGLIT ID
%token NIL NEW TYPE ARRAY OF VAR ASSIGNMENT FUNCTION
%token IF THEN ELSE WHILE FOR TO DO LET IN END BREAK
%token CLASS EXTENDS PRIMITIVE IMPORT METHOD
%token ERROR NOTEQUAL ELESS EGREATER

%destructor {delete $$;} <node>
%destructor {free($$);} <strVal>

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
program: exp         {programNode.reset(new ParentASTNode("program",nodeType::PROGRAM, {$1}, @1));}
 | decs              {programNode.reset(new ParentASTNode("program",nodeType::PROGRAM, {$1}, @1));}
;

/*Since id shows up so many places, we want it to be a node and not a string*/
id: ID                         {$$ = new TokenASTNode(ID, $1, @1); free($1);}
;

typeId: id                     {$$ = new ParentASTNode("type id",nodeType::TYPE_ID,{$1}, @1);} 
;

exp
/*Control structures */
 : IF exp THEN exp                  {$$ = new ParentASTNode("if then",nodeType::IF_THEN, {$2, $4}, @1);}
 | IF exp THEN exp ELSE exp         {$$ = new ParentASTNode("if then else",nodeType::IF_THEN, {$2, $4, $6}, @1);}
 | WHILE exp DO exp                 {$$ = new ParentASTNode("while do",nodeType::WHILE_DO, {$2, $4}, @1);}
 | FOR id ASSIGNMENT exp TO exp DO exp {$$ = new ParentASTNode("for to do",nodeType::FOR_TO_DO, {$2, $4, $6, $8}, @1);}
 | BREAK                            {$$ = new TokenASTNode(BREAK, "break", @1);}
 | LET IN END                       {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {nullptr, nullptr}, @1);}  
 | LET IN exps END                  {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {nullptr, $3}, @1);}
 | LET decs IN END                  {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {$2, nullptr}, @1);}
 | LET decs IN exps END             {$$ = new ParentASTNode("let in end", nodeType::LET_IN_END, {$2, $4}, @1);}
/* array and record creation */
 | id '['exp']' OF exp         {$$ = new ParentASTNode("Array", nodeType::ARRAY_DEC, {new ParentASTNode("type id",nodeType::TYPE_ID,{$1},@1), $3, $6}, @1);}
 | typeId '{' '}'              {$$ = new ParentASTNode("Record", nodeType::RECORD_DEC,{$1},@1);}
 | typeId '{' recs '}'         {$$ = new ParentASTNode("Record", nodeType::RECORD_DEC,{$1,$3},@1);}
/*Assignment */
 | lValue ASSIGNMENT exp            {$$ = new ParentASTNode("assignment",nodeType::ASSIGNMENT_, {$1, $3}, @1);}
 | id ASSIGNMENT exp                {$$ = new ParentASTNode("assignment",nodeType::ASSIGNMENT_, {$1, $3}, @1);}
 | opExp
;
/*=========HELPERS FOR EXP=============*/
opExp
/* Variables, filed, elements of an array */
 : lValue                      {$$ = $1;}
 | id                          {$$ = new ParentASTNode("Reference", nodeType::REFERENCE, {$1}, @1);}
/* function call */
 | id '(' ')'                  {$$ = new ParentASTNode("Call function", nodeType::CALL_FUNC,{$1}, @1);}
 | id '(' expList ')'          {$$ = new ParentASTNode("Call function", nodeType::CALL_FUNC,{$1,$3}, @1);}
/* multiple expressions */
 | '(' exps ')'                     {$$ = new ParentASTNode("sequence",nodeType::SEQUENCE, {$2}, @1);}
 | '(' ')'                          {$$ = new ParentASTNode("sequence",nodeType::SEQUENCE, {}, @1);} 
/* Operations */
 | '-' opExp                     {$$ = new ParentASTNode("Negate", nodeType::NEGATE,{$2}, @1);}
 | opExp '+' opExp                 {$$ = new ParentASTNode("Add (+)", nodeType::ADD,{$1,$3}, @1);}
 | opExp '-' opExp                 {$$ = new ParentASTNode("Subtract (-)", nodeType::SUB,{$1,$3}, @1);}
 | opExp '*' opExp                 {$$ = new ParentASTNode("Mulitply (*)", nodeType::MULT,{$1,$3}, @1);}
 | opExp '/' opExp                 {$$ = new ParentASTNode("Divide (/)", nodeType::DIV,{$1,$3}, @1);}
 | opExp '=' opExp                 {$$ = new ParentASTNode("Equal (=)", nodeType::EQUAL,{$1,$3}, @1);}
 | opExp NOTEQUAL opExp            {$$ = new ParentASTNode("Not Equal (<>)", nodeType::NOT_EQUAL,{$1,$3}, @1);}
 | opExp '>' opExp                 {$$ = new ParentASTNode("Greater (>)", nodeType::GREATER,{$1,$3}, @1);}
 | opExp '<' opExp                 {$$ = new ParentASTNode("Lesser (<)", nodeType::LESSER,{$1,$3}, @1);}
 | opExp EGREATER opExp            {$$ = new ParentASTNode("Equal or Greater (>=)", nodeType::EQ_GREATER,{$1,$3}, @1);}
 | opExp ELESS opExp               {$$ = new ParentASTNode("Equal or Less (<=)", nodeType::EQ_LESS,{$1,$3}, @1);}
 | opExp '&' opExp                 {$$ = new ParentASTNode("And (&)", nodeType::AND,{$1,$3}, @1);}
 | opExp '|' opExp                      {$$ = new ParentASTNode("or (|)",nodeType::OR, {$1, $3}, @1);}
/* Tokens */
 | NIL                         {$$ = new TokenASTNode(NIL, "nil", @1); }
 | INTLIT                      {$$ = new TokenASTNode(INTLIT, $1, @1); free($1);}
 | STRINGLIT                   {$$ = new TokenASTNode(STRINGLIT, $1, @1); free($1);}
;
recs
 : id '=' exp                    {$$ = new ParentASTNode("Record Value", nodeType::REC_VAL,{$1,$3}, @1);}
 | id '=' exp ',' recs           {$$ = new ParentASTNode("Record Value", nodeType::REC_VAL,{$1,$3,$5}, @1);}
;
/* exps separated by commas */
expList
 : exp                                      {$$ = $1;}
 | exp ',' expList                          {$$ = new ParentASTNode("exp list", nodeType::EXP_LIST, {$1, $3}, @1);}
;

/* Departure: I cover the single id case above and the id > 1 case here */
lValue 
 : id '.' id                                {$$ = new ParentASTNode("Reference", nodeType::REFERENCE,{$1,$3}, @1);}
 | lValue '.' id                            {$$ = new ParentASTNode("Reference", nodeType::REFERENCE,{$1,$3}, @1);}
 | lValue '[' exp ']'                       {$$ = new ParentASTNode("Array Reference", nodeType::ARRAY_REF,{$1,$3}, @1);}
 | id '[' exp ']'                           {$$ = new ParentASTNode("Array Reference", nodeType::ARRAY_REF,{$1,$3}, @1);}
;
/* exps separated by semicolons */
exps
 : exp                                     {$$ = $1;}  
 | exp ';' exps                            {$$ = new ParentASTNode("Sequence", nodeType::SEQUENCE, {$1, $3}, @1);}
 ;
/*Departure: I cover the decs = 0 case wherever decs is used */
decs
 : dec                                      {$$ = new ParentASTNode("decs", nodeType::DECS, {$1}, @1);} 
 | dec  decs                                {$$ = new ParentASTNode("decs", nodeType::DECS, {$1, $2}, @1);}
;
dec
 : TYPE id '=' ty                           {$$ = new ParentASTNode("type declation", nodeType::TYPE_DEC, {$2, $4}, @1);}
/*class definition NOTE: the id of what it extends is the third child*/
 | CLASS id '{' '}'                         {$$ = new ParentASTNode("class declaration", nodeType::CLASS_DEC, {$2}, @1);}
 | CLASS id '{' classFields '}'             {$$ = new ParentASTNode("class declaration", nodeType::CLASS_DEC, {$2, $4}, @1);}
 | CLASS id EXTENDS typeId '{' classFields '}'  {$$ = new ParentASTNode("class declaration w/ extends", nodeType::CLASS_DEC, {$2, $6, $4}, @1);}
/*variable declaration */
 | varDec                                   {$$ = $1;}
/*function declartation */
 | FUNCTION id '(' tyFields ')' '=' exp     {$$ = new ParentASTNode("function declaration", nodeType::FUNC_DEC, {$2, $4, $7}, @1);}
 | FUNCTION id '(' tyFields ')' ':' typeId '=' exp {$$ = new ParentASTNode("function declaration", nodeType::FUNC_DEC, {$2, $4, $9, $7}, @1);}
/*primitive declaration */
 | PRIMITIVE id '(' tyFields ')'            {$$ = new ParentASTNode("primitive declaration", nodeType::PRIM_DEC, {$2, $4}, @1);}
 | PRIMITIVE id '(' tyFields ')' ':' typeId {$$ = new ParentASTNode("primitive declaration", nodeType::PRIM_DEC, {$2, $4, $7}, @1);}
/*importing declarations*/
 | IMPORT STRINGLIT                         {$$ = new ParentASTNode("import", nodeType::IMPORT_, {new TokenASTNode(STRINGLIT, $2, @1)}, @1); free($2);}

; 
/*helper rules for decs */
varDec
 : VAR id ASSIGNMENT exp                    {$$ = new ParentASTNode("variable declaration", nodeType::VAR_DEC, {$2, $4}, @1);}
 | VAR id ':' typeId ASSIGNMENT exp         {$$ = new ParentASTNode("variable declaration", nodeType::VAR_DEC, {$2, $6, $4}, @1);}

classFields
 : classField                               {$$ = new ParentASTNode("class fields", nodeType::CLASS_FIELDS, {$1}, @1);}

 | classField classFields                   {$$ = new ParentASTNode("class fields", nodeType::CLASS_FIELDS, {$1, $2}, @1);}
;
classField
 : varDec                                   {$$ = new ParentASTNode("class field", nodeType::CLASS_FIELD, {$1}, @1);}
 | METHOD id '(' tyFields ')' '=' exp       {$$ = new ParentASTNode("class field Method Declaration", nodeType::CLASS_FIELD, {$2, $4, nullptr, $7}, @1);}

 | METHOD id '(' tyFields ')' ':' typeId '=' exp {$$ = new ParentASTNode("class field Method Declaration", nodeType::CLASS_FIELD, {$2, $4, $7, $9}, @1);}

ty
 : typeId                                   {$$ = $1;}
 /*Record type definition.*/
 | '{' tyFields '}'                         {$$ = new ParentASTNode("record type definition", nodeType::REC_TYPE, {$2}, @1);}
 /*Array type definition. */
 | ARRAY OF typeId                          {$$ = new ParentASTNode("array type definition", nodeType::ARRAY_TYPE, {$3}, @1);}
 /*Class definition (canonical form). */
 | CLASS '{' '}'                            {$$ = new ParentASTNode("class definition", nodeType::CLASS_TYPE, {}, @1);}
 | CLASS '{' classFields '}'                {$$ = new ParentASTNode("class definition", nodeType::CLASS_TYPE, {$3}, @1);}
 | CLASS EXTENDS typeId '{' '}'             {$$ = new ParentASTNode("class definition", nodeType::CLASS_TYPE, {nullptr, $3}, @1);}
 | CLASS EXTENDS typeId '{' classFields '}' {$$ = new ParentASTNode("class definition", nodeType::CLASS_TYPE, {$5, $3}, @1);}
;

tyFields :                                 {$$ = new ParentASTNode("type fields", nodeType::TY_FIELDS, {}, @0);} 
 | id ':' typeId                           {$$ = new ParentASTNode("type fields", nodeType::TY_FIELDS, {$1, $3}, @1);}

 | id ':' typeId ',' tyFields              {$$ = new ParentASTNode("type fields", nodeType::TY_FIELDS, {$1, $3, $5}, @1);}
;


%%
void yyerror (char const *s) {
    std::cerr << "Error at line " << yylineno << ": "; 
    std::cerr << s << std::endl;
}
