%{
	#include "token.hh"
%}

%option noyywrap
%option nounistd

%%
"array"	{return ARRAY;}
%%