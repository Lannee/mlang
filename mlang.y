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

mlang::expr_list *prog;

%}

// token type definition
%union {
	uint8_t int_val;
	char *str_val;
    std::vector<const mlang::expression *> *expr_list_t;
    const mlang::expression *expr_t;
    const mlang::statement *stmt_t;
    const mlang::type *value_t;
}

// constant tokens
%token ASSIGNMENT
%token BGN END COMMA IF ELSE
%token LET
%token EQUAL
%token UNIT
%token PRINT

// terminal symbols
%token <int_val> INT
%token <str_val> STRING
%token <str_val> VAR

// non-terminal symbols
%type <stmt_t> stmt
%type <expr_list_t> expr_list
%type <expr_list_t> exprs args
%type <expr_t> expr
%type <value_t> value

%start program

%%

program
	: exprs                           { prog = new mlang::expr_list($1); }
;

exprs
    : /* empty */                     { $$ = new std::vector<const mlang::expression *>(0); }
	| exprs expr                      { $1->push_back($2); }
;

expr
    : value                           { $$ = $1; }
    | VAR                             { $$ = new mlang::variable($1); free($1); }
    | BGN exprs END                   { $$ = new mlang::expr_list($2); }
    | LET VAR ASSIGNMENT expr         { $$ = new mlang::var_decl($2, $4); free($2); }
    | IF expr expr ELSE expr          { $$ = new mlang::if_expression($2, $3, $5); }
    | IF expr expr                    { $$ = new mlang::if_expression($2, $3, nullptr); }
    | stmt
;

stmt
    : PRINT args                      { $$ = new mlang::print_statement($2); }
;

args
    : expr                            { $$ = new std::vector<const mlang::expression *>(1, $1); }
    | args COMMA expr                 { $1->push_back($3); }
;

value
    : INT                     { $$ = new mlang::integer_type($1); }
    | STRING                  { $$ = new mlang::string_type($1); free($1); }
    | UNIT                    { $$ = &mlang::UNIT__; }
;

%%

int main(int argc, char **argv){
    yyparse();

    mlang::context ctx{};

    auto *ret = prog->value(ctx);

    int status = ret->to_integer_type().data__();

    delete prog;
	return status;
}

void yyerror(const char *s) {
	std::cout << "Error: " << s << std::endl;
}
