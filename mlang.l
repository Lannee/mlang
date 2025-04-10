%{

#include <cstdio>
#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>

#include "ast.hh"

#include "parser.tab.hh"

%}

ALPHA    [A-Za-z]
DIGIT    [0-9]

%%

[ \t\n]	        {}
{DIGIT}+		{ yylval.int_val = atoi(yytext); return INT; }
\"[^\"\n]*\"	{	// remove the quote marks from either end
					yylval.str_val = (char *)malloc(sizeof(char)*strlen(yytext)-1);
					strncpy(yylval.str_val, yytext+1, strlen(yytext)-2);
					yylval.str_val[strlen(yytext)-2] = '\0';	// terminate the string!
					return STRING;
				}
#               { return HASH; }
print           { return PRINT; }
let				{ return LET; }
begin 			{ return BGN; }
end				{ return END; }
\=\?			{ return EQUAL; }
\=				{ return ASSIGNMENT; }
T               { return UNIT; }
{ALPHA}({ALPHA}|{DIGIT})*   { yylval.str_val = strdup(yytext); return VAR; }
.	            { return 0; }
%%

int yywrap() {
    return 1;
}
