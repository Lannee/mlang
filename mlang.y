%{
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>

extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;
extern "C" int yywrap();

void yyerror(const char *s);

#include "expression.hh"

%}

// token type definition
%union {
	int int_val;
	char *str_val;
	std::vector<std::string> *sList;
    mlang::type *type_val;
}

// constant tokens
%token WRITELN
%token ASSIGNMENT
%token ENDL

// terminal symbols
%token <int_val> INT
%token <str_val> STRING
%token <type_val> TYPE
%token <str_val> VAR

// non-terminal symbols
%type <str_val> comp
%type <sList> stringList

%%

input:
	/* empty */
	| input line
;

line:
	ENDL
	| stmt ENDL
;

stmt:
    expr
    | WRITELN expr			    { $$ = new Writeln($2); }
;

expr:
    type_value
    | BEGIN expr_list END             { $$ = }
    | LET VAR ASSIGNMENT expr
;

expr_list:
    expr                              { $$ = new std::vector<expression*>(1, $1); }
    | expr_list expr_list_sep expr    { 
                                        $1->push_back($3);
 								        $$ = $1;
                                      }
;

expr_list_sep:
    ENDL
    | END
;

typed_value:
    INT                     { $$ = int_val; }
;

%%

int main(int argc, char **argv){
    yyparse();
	
	return 0;
}

void yyerror(const char *s){
	std::cout << "Error: " << s << std::endl;
}

int yywrap(){
	if( yyin == stdin )
		return 1;
	fclose(yyin);
	yyin = stdin;
	return 0;
}