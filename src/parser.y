%{
#include <stdio.h>

#include "common.h"
#include "type.h"
#include "syntax.h"

void yyerror(const char *s);
int yylex(void);
%}

%union {
    int int_val;
    float float_val;
    char char_val;
    char *string_val;
    A_NODE *node;
    A_TYPE *type;
    A_ID *id;
    A_SPECIFIER *spec;
    S_KIND s;
    T_KIND t;
}

%start program

%token <int_val> INTEGER_CONSTANT
%token <float_val> FLOAT_CONSTANT
%token <char_val> CHARACTER_CONSTANT
%token <string_val> STRING_LITERAL
%token <string_val> IDENTIFIER
%token <type> TYPE_IDENTIFIER
%token PLUS
%token MINUS
%token PLUSPLUS
%token MINUSMINUS
%token BAR
%token AMP
%token BARBAR
%token AMPAMP
%token ARROW
%token SEMICOLON
%token LSS
%token GTR
%token LEQ
%token GEQ
%token EQL
%token NEQ
%token DOTDOTDOT
%token LP
%token RP
%token LB
%token RB
%token LR
%token RR
%token PERIOD
%token COMMA
%token EXCL
%token STAR
%token SLASH
%token PERCENT
%token ASSIGN
%token COLON
%token AUTO_SYM
%token STATIC_SYM
%token TYPEDEF_SYM
%token STRUCT_SYM
%token ENUM_SYM
%token SIZEOF_SYM
%token UNION_SYM
%token IF_SYM
%token ELSE_SYM
%token WHILE_SYM
%token DO_SYM
%token FOR_SYM
%token CONTINUE_SYM
%token BREAK_SYM
%token RETURN_SYM
%token SWITCH_SYM
%token CASE_SYM
%token DEFAULT_SYM

%type <node> program
%type <id> translation_unit external_declaration function_definition
%type <spec> declaration_specifiers
%type <type> type_specifier struct_type_specifier enum_type_specifier type_name
%type <id> declaration_list_opt declaration_list declaration init_declarator_list_opt init_declarator init_declarator_list
%type <id> declarator direct_declarator
%type <node> initializer initializer_list
%type <type> pointer
%type <type> abstract_declarator_opt abstract_declarator direct_abstract_declarator
%type <id> parameter_type_list_opt parameter_type_list
%type <id> parameter_list parameter_declaration
%type <node> compound_statement statement statement_list statement_list_opt
%type <node> labeled_statement selection_statement iteration_statement jump_statement
%type <node> expression_statement
%type <node> for_expression expression_opt
%type <node> expression comma_expression
%type <node> assignment_expression conditional_expression
%type <node> logical_or_expression logical_and_expression
%type <node> bitwise_or_expression bitwise_xor_expression bitwise_and_expression
%type <node> equality_expression relational_expression shift_expression
%type <node> additive_expression multiplicative_expression
%type <node> cast_expression unary_expression postfix_expression primary_expression
%type <node> constant_expression constant_expression_opt
%type <node> arg_expression_list arg_expression_list_opt
%type <id> struct_declaration_list struct_declaration
%type <id> struct_declarator struct_declarator_list
%type <id> enumerator_list enumerator
%type <t> struct_or_union
%type <s> storage_class_specifier


%expect 1

%%

program
    : translation_unit { root = makeNode(N_PROGRAM, NULL, $1, NULL); checkForwardReference(); }
    ;
translation_unit
    : external_declaration { $$ = $1; }
    | translation_unit external_declaration { $$ = linkDeclaratorList($1, $2); }
    ;
external_declaration
    : function_definition { $$ = $1; }
    | declaration { $$ = $1; }
    ;
function_definition
    : declaration_specifiers declarator { $<id>$ = setFunctionDeclaratorSpecifier($2, $1); } compound_statement { $$ = setFunctionDeclaratorBody($<id>3, $4); }
    | declarator { $<id>$ = setFunctionDeclaratorSpecifier($1, makeSpecifier(int_type, S_NULL)); } compound_statement { $$ = setFunctionDeclaratorBody($<id>2, $3); }
    ;
declaration_list_opt
    : /* empty */ { $$ = NULL; }
    | declaration_list { $$ = $1; }
    ;
declaration_list
    : declaration { $$ = $1; }
    | declaration_list declaration { $$ = linkDeclaratorList($1, $2); }
    ;
declaration
    : declaration_specifiers init_declarator_list_opt SEMICOLON { $$ = setDeclaratorListSpecifier($2, $1); }
    ;
init_declarator_list_opt
    : /* empty */ { $$ = makeDummyIdentifier(); }
    | init_declarator_list { $$ = $1; }
    ;
init_declarator_list
    : init_declarator { $$ = $1; }
    | init_declarator_list COMMA init_declarator { $$ = linkDeclaratorList($1, $3); }
    ;
init_declarator
    : declarator { $$ = $1; }
    | declarator ASSIGN initializer { $$ = setDeclaratorInit($1, $3); }
    ;
initializer_list
    : initializer { $$ = makeNode(N_INIT_LIST, $1, NULL, makeNode(N_INIT_LIST_NIL, NULL, NULL, NULL)); }
    | initializer_list COMMA initializer { $$ = makeNodeList(N_INIT_LIST, $1, $3); }
    ;
initializer
    : constant_expression { $$ = makeNode(N_INIT_LIST_ONE, NULL, $1, NULL); }
    | LR initializer_list RR { $$ = $2; }
    ;
declaration_specifiers
    : type_specifier { $$ = makeSpecifier($1, S_NULL); }
    | storage_class_specifier { $$ = makeSpecifier(NULL, $1); }
    | type_specifier declaration_specifiers { $$ = updateSpecifier($2, $1, S_NULL); }
    | storage_class_specifier declaration_specifiers { $$ = updateSpecifier($2, NULL, $1); }
    ;
storage_class_specifier
    : AUTO_SYM { $$ = S_AUTO; }
    | STATIC_SYM { $$ = S_STATIC; }
    | TYPEDEF_SYM { $$ = S_TYPEDEF; }
    ;
type_specifier
    : struct_type_specifier { $$ = $1; }
    | enum_type_specifier { $$ = $1; }
    | TYPE_IDENTIFIER { $$ = $1; }
    ;
struct_type_specifier
    : struct_or_union IDENTIFIER  { $<type>$ = setTypeStructOrEnumIdentifier($1, $2, ID_STRUCT); } LR { $<id>$ = current_id; current_level++; } struct_declaration_list RR { checkForwardReference(); $$ = setTypeField($<type>3, $6); current_level--; current_id = $<id>5; }
    | struct_or_union { $<type>$ = makeType($1); } LR { $<id>$ = current_id; current_level++; } struct_declaration_list RR { checkForwardReference(); $$ = setTypeField($<type>2, $5); current_level--; current_id = $<id>4; }
    | struct_or_union IDENTIFIER { $$ = getTypeOfStructOrEnumRefIdentifier($1, $2, ID_STRUCT); }
    ;
struct_or_union
    : STRUCT_SYM { $$ = T_STRUCT; }
    | UNION_SYM { $$ = T_UNION; }
    ;
struct_declaration_list
    : struct_declaration { $$ = $1; }
    | struct_declaration_list struct_declaration { $$ = linkDeclaratorList($1, $2); }
    ;
struct_declaration
    : type_specifier struct_declarator_list SEMICOLON { $$ = setStructDeclaratorListSpecifier($2, $1); }
    ;
struct_declarator_list
    : struct_declarator { $$ = $1; }
    | struct_declarator_list COMMA struct_declarator { $$ = linkDeclaratorList($1, $3); }
    ;
struct_declarator
    : declarator { $$ = $1; }
    ;
enum_type_specifier
    : ENUM_SYM IDENTIFIER { $<type>$ = setTypeStructOrEnumIdentifier(T_ENUM, $2, ID_ENUM); } LR enumerator_list RR { $$ = setTypeField($<type>3, $5); }
    | ENUM_SYM { $<type>$ = makeType(T_ENUM); } LR enumerator_list RR { $$ = setTypeField($<type>2, $4); }
    | ENUM_SYM IDENTIFIER { $$ = getTypeOfStructOrEnumRefIdentifier(T_ENUM, $2, ID_ENUM); }
    ;
enumerator_list
    : enumerator { $$ = $1; }
    | enumerator_list COMMA enumerator { $$ = linkDeclaratorList($1, $3); }
    ;
enumerator
    : IDENTIFIER { $$ = setDeclaratorKind(makeIdentifier($1), ID_ENUM_LITERAL); }
    | IDENTIFIER { $<id>$ = setDeclaratorKind(makeIdentifier($1), ID_ENUM_LITERAL); } ASSIGN expression { $$ = setDeclaratorInit($<id>2, $4); }
    ;
declarator
    : pointer direct_declarator { $$ = setDeclaratorElementType($2, $1); }
    | direct_declarator { $$ = $1; }
    ;
pointer
    : STAR { $$ = makeType(T_POINTER); }
    | STAR pointer { $$ = setTypeElementType($2, makeType(T_POINTER)); }
    ;
direct_declarator
    : IDENTIFIER { $$ = makeIdentifier($1); }
    | LP declarator RP { $$ = $2; }
    | direct_declarator LB constant_expression_opt RB { $$ = setDeclaratorElementType($1, setTypeExpr(makeType(T_ARRAY), $3)); }
    | direct_declarator LP { $<id>$ = current_id; current_level++; } parameter_type_list_opt RP {checkForwardReference(); current_id = $<id>3; current_level--; $$ = setDeclaratorElementType($1, setTypeField(makeType(T_FUNC), $4)); }
    ;
parameter_type_list_opt
    : /* empty */ { $$ = NULL; }
    | parameter_type_list { $$ = $1; }
    ;
parameter_type_list
    : parameter_list { $$ = $1; }
    | parameter_list COMMA DOTDOTDOT { $$ = linkDeclaratorList($1, setDeclaratorKind(makeDummyIdentifier(), ID_PARM)); }
    ;
parameter_list
    : parameter_declaration { $$ = $1; }
    | parameter_list COMMA parameter_declaration { $$ = linkDeclaratorList($1, $3); }
    ;
parameter_declaration
    : declaration_specifiers declarator { $$ = setParameterDeclaratorSpecifier($2, $1); }
    | declaration_specifiers abstract_declarator_opt { $$ = setParameterDeclaratorSpecifier(setDeclaratorType(makeDummyIdentifier(), $2), $1); }
    ;
abstract_declarator_opt
    : /* empty */ { $$ = NULL; }
    | abstract_declarator { $$ = $1; }
    ;
abstract_declarator
    : direct_abstract_declarator { $$ = $1; }
    | pointer { $$ = makeType(T_POINTER); }
    | pointer direct_abstract_declarator { $$ = setTypeElementType($2, makeType(T_POINTER)); }
    ;
direct_abstract_declarator
    : LP abstract_declarator RP { $$ = $2; }
    | LB constant_expression_opt RB { $$ = setTypeExpr(makeType(T_ARRAY), $2); }
    | direct_abstract_declarator LB constant_expression_opt RB { $$ = setTypeElementType($1, setTypeExpr(makeType(T_ARRAY), $3)); }
    | LP parameter_type_list_opt RP { $$ = setTypeField(makeType(T_FUNC), $2); }
    | direct_abstract_declarator LP parameter_type_list_opt RP { $$ = setTypeElementType($1, setTypeField(makeType(T_FUNC), $3)); }
    ;
statement_list_opt
    : /* empty */ { $$ = makeNode(N_STMT_LIST_NIL, NULL, NULL, NULL); }
    | statement_list { $$ = $1; }
    ;
statement_list
    : statement { $$ = makeNode(N_STMT_LIST, $1, NULL, makeNode(N_STMT_LIST_NIL, NULL, NULL, NULL)); }
    | statement_list statement { $$ = makeNodeList(N_STMT_LIST, $1, $2); }
    ;
statement
    : labeled_statement { $$ = $1; }
    | compound_statement { $$ = $1; }
    | expression_statement { $$ = $1; }
    | selection_statement { $$ = $1; }
    | iteration_statement { $$ = $1; }
    | jump_statement { $$ = $1; }
    ;
labeled_statement
    : CASE_SYM constant_expression COLON statement { $$ = makeNode(N_STMT_LABEL_CASE, $2, NULL, $4); }
    | DEFAULT_SYM COLON statement { $$ = makeNode(N_STMT_LABEL_DEFAULT, NULL, $3, NULL); }
    ;
compound_statement
    : LR { $<id>$ = current_id; current_level++; } declaration_list_opt statement_list_opt RR { checkForwardReference(); $$ = makeNode(N_STMT_COMPOUND, $3, NULL, $4); current_id = $<id>2; current_level--; }
    ;
expression_statement
    : SEMICOLON { $$ = makeNode(N_STMT_EMPTY, NULL, NULL, NULL); }
    | expression SEMICOLON { $$ = makeNode(N_STMT_EXPRESSION, NULL, $1, NULL); }
    ;
selection_statement
    : IF_SYM LP expression RP statement { $$ = makeNode(N_STMT_IF, $3, NULL, $5); }
    | IF_SYM LP expression RP statement ELSE_SYM statement { $$ = makeNode(N_STMT_IF_ELSE, $3, $5, $7); }
    | SWITCH_SYM LP expression RP statement { $$ = makeNode(N_STMT_SWITCH, $3, NULL, $5); }
    ;
iteration_statement
    : WHILE_SYM LP expression RP statement { $$ = makeNode(N_STMT_WHILE, $3, NULL, $5); }
    | DO_SYM statement WHILE_SYM LP expression RP SEMICOLON { $$ = makeNode(N_STMT_DO, $2, NULL, $5); }
    | FOR_SYM LP for_expression RP statement { $$ = makeNode(N_STMT_FOR, $3, NULL, $5); }
    ;
for_expression
    : expression_opt SEMICOLON expression_opt SEMICOLON expression_opt { $$ = makeNode(N_FOR_EXP, $1, $3, $5); }
    ;
expression_opt
    : /* empty */ { $$ = NULL; }
    | expression { $$ = $1; }
    ;
jump_statement
    : RETURN_SYM expression_opt SEMICOLON { $$ = makeNode(N_STMT_RETURN, NULL, $2, NULL); }
    | CONTINUE_SYM SEMICOLON { $$ = makeNode(N_STMT_CONTINUE, NULL, NULL, NULL); }
    | BREAK_SYM SEMICOLON { $$ = makeNode(N_STMT_BREAK, NULL, NULL, NULL); }
    ;
arg_expression_list_opt
    : /* empty */ { $$ = makeNode(N_ARG_LIST_NIL, NULL, NULL, NULL); }
    | arg_expression_list { $$ = $1; }
    ;
arg_expression_list
    : assignment_expression { $$ = makeNode(N_ARG_LIST, $1, NULL, makeNode(N_ARG_LIST_NIL, NULL, NULL, NULL)); }
    | arg_expression_list COMMA assignment_expression { $$ = makeNodeList(N_ARG_LIST, $1, $3); }
    ;
constant_expression_opt
    : /* empty */ { $$ = NULL; }
    | constant_expression { $$ = $1; }
    ;
constant_expression
    : expression { $$ = $1; }
    ;
expression
    : comma_expression { $$ = $1; }
    ;
comma_expression
    : assignment_expression { $$ = $1; }
    ;
assignment_expression
    : conditional_expression { $$ = $1; }
    | unary_expression ASSIGN assignment_expression { $$ = makeNode(N_EXP_ASSIGN, $1, NULL, $3); }
    ;
conditional_expression
    : logical_or_expression { $$ = $1; }
    ;
logical_or_expression
    : logical_and_expression { $$ = $1; }
    | logical_or_expression BARBAR logical_and_expression { $$ = makeNode(N_EXP_OR, $1, NULL, $3); }
    ;
logical_and_expression
    : bitwise_or_expression { $$ = $1; }
    | logical_and_expression AMPAMP bitwise_or_expression { $$ = makeNode(N_EXP_AND, $1, NULL, $3); }
    ;
bitwise_or_expression
    : bitwise_xor_expression { $$ = $1; }
    ;
bitwise_xor_expression
    : bitwise_and_expression { $$ = $1; }
    ;
bitwise_and_expression
    : equality_expression { $$ = $1; }
    ;
equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression EQL relational_expression { $$ = makeNode(N_EXP_EQL, $1, NULL, $3); }
    | equality_expression NEQ relational_expression { $$ = makeNode(N_EXP_NEQ, $1, NULL, $3); }
    ;
relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression LSS shift_expression { $$ = makeNode(N_EXP_LSS, $1, NULL, $3); }
    | relational_expression GTR shift_expression { $$ = makeNode(N_EXP_GTR, $1, NULL, $3); }
    | relational_expression LEQ shift_expression { $$ = makeNode(N_EXP_LEQ, $1, NULL, $3); }
    | relational_expression GEQ shift_expression { $$ = makeNode(N_EXP_GEQ, $1, NULL, $3); }
    ;
shift_expression
    : additive_expression { $$ = $1; }
    ;
additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression PLUS multiplicative_expression { $$ = makeNode(N_EXP_ADD, $1, NULL, $3); }
    | additive_expression MINUS multiplicative_expression { $$ = makeNode(N_EXP_SUB, $1, NULL, $3); }
    ;
multiplicative_expression
    : cast_expression { $$ = $1; }
    | multiplicative_expression STAR cast_expression { $$ = makeNode(N_EXP_MUL, $1, NULL, $3); }
    | multiplicative_expression SLASH cast_expression { $$ = makeNode(N_EXP_DIV, $1, NULL, $3); }
    | multiplicative_expression PERCENT cast_expression { $$ = makeNode(N_EXP_MOD, $1, NULL, $3); }
    ;
cast_expression
    : unary_expression { $$ = $1; }
    | LP type_name RP cast_expression { $$ = makeNode(N_EXP_CAST, $2, NULL, $4); }
    ;
unary_expression
    : postfix_expression { $$ = $1; }
    | PLUSPLUS unary_expression { $$ = makeNode(N_EXP_PRE_INC, NULL, $2, NULL); }
    | MINUSMINUS unary_expression { $$ = makeNode(N_EXP_PRE_DEC, NULL, $2, NULL); }
    | AMP cast_expression { $$ = makeNode(N_EXP_AMP, NULL, $2, NULL); }
    | STAR cast_expression { $$ = makeNode(N_EXP_STAR, NULL, $2, NULL); }
    | EXCL cast_expression { $$ = makeNode(N_EXP_NOT, NULL, $2, NULL); }
    | MINUS cast_expression { $$ = makeNode(N_EXP_MINUS, NULL, $2, NULL); }
    | PLUS cast_expression { $$ = makeNode(N_EXP_PLUS, NULL, $2, NULL); }
    | SIZEOF_SYM unary_expression { $$ = makeNode(N_EXP_SIZE_EXP, NULL, $2, NULL); }
    | SIZEOF_SYM LP type_name RP { $$ = makeNode(N_EXP_SIZE_TYPE, NULL, $3, NULL); }
    ;
postfix_expression
    : primary_expression { $$ = $1; }
    | postfix_expression LB expression RB { $$ = makeNode(N_EXP_ARRAY, $1, NULL, $3); }
    | postfix_expression LP arg_expression_list_opt RP { $$ = makeNode(N_EXP_FUNCTION_CALL, $1, NULL, $3); }
    | postfix_expression PERIOD IDENTIFIER { $$ = makeNode(N_EXP_STRUCT, $1, NULL, $3); }
    | postfix_expression ARROW IDENTIFIER { $$ = makeNode(N_EXP_ARROW, $1, NULL, $3); }
    | postfix_expression PLUSPLUS { $$ = makeNode(N_EXP_POST_INC, NULL, $1, NULL); }
    | postfix_expression MINUSMINUS { $$ = makeNode(N_EXP_POST_DEC, NULL, $1, NULL); }
    ;
primary_expression
    : IDENTIFIER { $$ = makeNode(N_EXP_IDENT, NULL, getIdentifierDeclared($1), NULL); }
    | INTEGER_CONSTANT { $$ = setNodeInteger(makeNode(N_EXP_INT_CONST, NULL, NULL, NULL), $1); }
    | FLOAT_CONSTANT { $$ = setNodeFloat(makeNode(N_EXP_FLOAT_CONST, NULL, NULL, NULL), $1); }
    | CHARACTER_CONSTANT { $$ = setNodeChar(makeNode(N_EXP_CHAR_CONST, NULL, NULL, NULL), $1); }
    | STRING_LITERAL { $$ = setNodeString(makeNode(N_EXP_STRING_LITERAL, NULL, NULL, NULL), $1); }
    | LP expression RP { $$ = $2; }
    ;
type_name
    : declaration_specifiers abstract_declarator_opt { $$ = setTypeNameSpecifier($2, $1); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
    exit(1);
}
