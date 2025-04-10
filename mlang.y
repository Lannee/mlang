%require "3.2"

%{
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>

#include "ast.hh"

int yylex();
int yyparse();

void yyerror(const char *s);

std::vector<mlang::expression *> *prog;

%}

// token type definition
%union {
	uint8_t int_val;
	char *str_val;
    std::vector<mlang::expression *> *expr_list_t;
    mlang::expression *expr_t;
    mlang::statement *stmt_t;
    mlang::type *value_t;
}

// constant tokens
%token ASSIGNMENT
%token BGN END HASH
%token LET
%token EQUAL

// terminal symbols
%token <int_val> INT
%token <str_val> STRING
%token <str_val> VAR

// non-terminal symbols
%type <stmt_t> stmt
%type <expr_list_t> expr_list
%type <expr_t> expr
%type <value_t> value

%start program

%%

program
	: /* empty */               { prog = new std::vector<mlang::expression *>(0); }
	| program expr              { prog->push_back($2); }
;

expr
    : value                           { $$ = $1; }
    | BGN expr_list END               { $$ = new mlang::expr_list($2); }
    | LET VAR ASSIGNMENT expr         { $$ = new mlang::var_decl($2, $4); }
;

expr_list
    : expr                            { $$ = new std::vector<mlang::expression *>(1, $1); }
    | expr_list HASH expr             { 
                                        $1->push_back($3);
 								        $$ = $1;
                                      }
;

value
    : INT                     { $$ = new mlang::integer_type($1); }
;

%%

int main(int argc, char **argv){
    yyparse();

    mlang::context ctx{};

    for(auto s : *prog) {
        std::cout << "a" << std::endl;
        s->value(ctx);
    }

	return 0;
}

void yyerror(const char *s) {
	std::cout << "Error: " << s << std::endl;
}
