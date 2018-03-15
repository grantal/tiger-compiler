%{
        #include "ast.hh" 
	#include "tiger.tab.h"
	#include <unistd.h>
	int commentDepth = 0;
        extern YYSTYPE yylval;
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
\"(\\[abfnrtv\\\"]|\\x[0-9A-Fa-f]{2}|\\[0-3][0-7]{2}|[^\\\"\n\r])*\" {yylval.strVal = strdup(yytext); return STRINGLIT;} 
[0-9]+				{yylval.strVal = strdup(yytext); return INTLIT;}
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
method				{return METHOD;}
new				{return NEW;}
,  | 				
:  |     			
;  |     		        
\( |     			
\) |     			
\[ |     			
\] |     			
\{ |     			
\} |     			
\. |     			
\+ |     			
-  |     			
\* |     			
\/ |     			
=  |
\< |
\> |
&  |
\|                              {return yytext[0];}      			
\<\>				{return NOTEQUAL;}
\<=				{return ELESS;}
\>= 				{return EGREATER;}
:=				{return ASSIGNMENT;}
\n\r|\r\n|\r|\n			{return ENDL;}
[a-zA-Z][A-Za-z0-9_]*|_main     {yylval.strVal = strdup(yytext); return ID;}
[ \t]*                          {}
.                               {return ERROR;}
%%
