%{
	#include "token.hh"
	int commentDepth = 0;
%}

%option noyywrap
%option nounistd
%x commentState
%%

\/\*				{commentDepth++;
				 BEGIN(commentState);}
<commentState>\/\*	        {commentDepth++;
				 BEGIN(commentState);}
<commentState>\*\/	        {commentDepth--;
				 if(commentDepth <= 0){
				    commentDepth = 0;
				    BEGIN(INITIAL);
				 }else{
				    BEGIN(commentState);
				 }
				}	
<commentState>.|\n|\r		{}
\"(\\[abfnrtv\\\"]|\\x[0-9A-Fa-f]{2}|\\[0-3][0-7]{2}|[^\\\"\n\r])*\" {return STRINGLIT;} 
[0-9]+				{return INTLIT;}
array				{return ARRAY;}
if 				{return IF;}
then 				{return THEN;}
else				{return ELSE;}
while				{return WHILE;}
for				{return FOR;}
to				{return TO;}
do				{return DO;}
let				{return LET;}
in				{return IN;}
end				{return END;}
of				{return OF;}
break				{return BREAK;}
nil				{return NIL;}
function			{return FUNCTION;}
var				{return VAR;}
type				{return TYPE;}
import				{return IMPORT;}
primitive			{return PRIMITIVE;}
class				{return CLASS;}
extends				{return EXTENDS;}
methods				{return METHODS;}
new				{return NEW;}
,				{return COMMA;}
:				{return COLON;}
;				{return SEMICOLON;}
\(				{return LPAREN;}
\)				{return RPAREN;}
\[				{return LBRACKET;}
\]				{return RBRACKET;}
\{				{return LBRACE;}
\}				{return RBRACE;}
\.				{return DOT;}
\+				{return PLUS;}
-				{return MINUS;}
\*				{return STAR;}
\/				{return FSLASH;}
= 				{return EQUAL;}
\<\>				{return NOTEQUAL;}
\<				{return LESS;}
\>				{return GREATER;}
\<=				{return ELESS;}
\>= 				{return EGREATER;}
& 				{return AND;}
\|				{return OR;}
:=				{return ASSIGNMENT;}
\n\r|\r\n|\r|\n			{return ENDL;}
[a-zA-Z][A-Za-z0-9_]*|_main     {return IDENTIFIER;}
[ \t]*                          {}
.                               {return ERROR;}
%%
