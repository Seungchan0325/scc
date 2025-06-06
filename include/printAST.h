#ifndef PRINTAST_H

#define PRINTAST_H

#include "type.h"

extern char *node_name[];
extern char *type_kind_name[];
extern char *id_kind_name[];
extern char *spec_name[];

void print_node(A_NODE *node, int s);
void print_space(int s);
void print_ast(A_NODE *node);
void prt_program(A_NODE *node, int s);
void prt_initializer(A_NODE *node, int s);
void prt_expression(A_NODE *node, int s);
void prt_arg_expr_list(A_NODE *node, int s);
void prt_statement(A_NODE *node, int s);
void prt_statement_list(A_NODE *node, int s);
void prt_for_expression(A_NODE *node, int s);
void prt_integer(int a, int s);
void prt_float(float a, int s);
void prt_char(char c, int s);
void prt_STRING(char *str, int s);
void prt_A_TYPE(A_TYPE *t, int s);
void prt_A_ID_LIST(A_ID *id, int s);
void prt_A_ID_NAME(A_ID *id, int s);
void prt_A_ID(A_ID *id, int s);

void printProgram(A_NODE *node, int s);
void printStatement(A_NODE *node, int s);
void printStatement_list(A_NODE *node, int s);
void printExpression(A_NODE *node);
void printA_ID_LIST(A_ID *id_list, int s);
void printA_ID(A_ID *id, int s);
void printA_TYPE(A_TYPE *type);
void printArg_expr_list(A_NODE *node);
void printIntializer(A_NODE *node);
void printFor_expression(A_NODE *node);

#endif