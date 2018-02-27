%{
	#include "token.hh"
%}

%option noyywrap
%option nounistd

%%
array	{return ARRAY;}
[A-Za-z][A-Za-z0-9_]*|_main {return IDENTIFIER;}
.* {return ERROR;}
%%
