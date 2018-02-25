%{
	#include "token.hh"
%}

%option noyywrap
%option nounistd

%%
"IF"	{return IF;}
%%