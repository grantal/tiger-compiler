%{
	#include "token.hh";}
%}

%option noyywrap
%option nounistd

%%


(\/\*)[.]*(\*\/) {}
(\")(.)*(\")	{return STRINGLIT;}
[0-9]+			{return INTLIT;}
array			{return ARRAY;}
if 				{return IF;}
then 			{return THEN;}
else			{return ELSE;}
while			{return WHILE;}
for				{return FOR;}
to				{return TO;}
do				{return DO;}
let				{return LET;}
in				{return IN;}
end				{return END;}
of				{return OF;}
break			{return BREAK;}
nil				{return NIL;}
function		{return FUNCTION;}
var				{return VAR;}
type			{return TYPE;}
import			{return IMPORT;}
primitive		{return PRIMITIVE;}
class			{return CLASS;}
extends			{return EXTENDS;}
methods			{return METHODS;}
new				{return NEW;}

%%