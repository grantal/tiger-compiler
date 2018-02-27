%{
	#include "token.hh";}
%}

%option noyywrap
%option nounistd

%%

(\/\*)[.]*(\*\/)                {}
(\")(.)*(\")	                {return STRINGLIT;}
[0-9]+			        {return INTLIT;}
array			        {return ARRAY;}
if 				{return IF;}
then 			        {return THEN;}
else			        {return ELSE;}
while			        {return WHILE;}
for				{return FOR;}
to				{return TO;}
do				{return DO;}
let				{return LET;}
in				{return IN;}
end				{return END;}
of				{return OF;}
break			        {return BREAK;}
nil				{return NIL;}
function		        {return FUNCTION;}
var				{return VAR;}
type			        {return TYPE;}
import			        {return IMPORT;}
primitive		        {return PRIMITIVE;}
class			        {return CLASS;}
extends			        {return EXTENDS;}
methods			        {return METHODS;}
new			        {return NEW;}
,				{return COMMA;}
:                               {return COLON;}
;                               {return SEMICOLON;}
\(				{return LPAREN;}
\)                              {return RPAREN;}
\[                              {return LBRACKET;}
\]                              {return RBRACKET;}
{				{return LBRACE;}
}                               {return RBRACE;}
\.                              {return DOT;}
\+                              {return PLUS;}
-                               {return MINUS;}
\*                              {return STAR;}
\/				{return FSLASH;}
=                               {return EQUAL;}
\<\>                            {return NOTEQUAL;}
\<                              {return LESS;}
\>                              {return GREATER;}
\<=				{return ELESS;}
\>=                             {return EGREATER;}
&                               {return AND;}
\|                              {return OR;}
:=				{return ASSIGNMENT;}
\\n\\r|\\r\\n                   {return ENDL;}
\\n|\\r                         {return ENDL;}
".*"                            {return STRINGLIT;}
[A-Za-z][A-Za-z0-9_]*|_main     {return IDENTIFIER;}
.*                              {return ERROR;}

%%
