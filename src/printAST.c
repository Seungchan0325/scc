#include <stdio.h>
#include <assert.h>

#include "common.h"
#include "type.h"
#include "printAST.h"
#include "syntax.h"

char *node_name[] = {
    "N_NULL",
    "N_PROGRAM",
    "N_EXP_IDENT",
    "N_EXP_INT_CONST",
    "N_EXP_FLOAT_CONST",
    "N_EXP_CHAR_CONST",
    "N_EXP_STRING_LITERAL",
    "N_EXP_ARRAY",
    "N_EXP_FUNCTION_CALL",
    "N_EXP_STRUCT",
    "N_EXP_ARROW",
    "N_EXP_POST_INC",
    "N_EXP_POST_DEC",
    "N_EXP_PRE_INC",
    "N_EXP_PRE_DEC",
    "N_EXP_AMP",
    "N_EXP_STAR",
    "N_EXP_NOT",
    "N_EXP_PLUS",
    "N_EXP_MINUS",
    "N_EXP_SIZE_EXP",
    "N_EXP_SIZE_TYPE",
    "N_EXP_CAST",
    "N_EXP_MUL",
    "N_EXP_DIV",
    "N_EXP_MOD",
    "N_EXP_ADD",
    "N_EXP_SUB",
    "N_EXP_LSS",
    "N_EXP_GTR",
    "N_EXP_LEQ",
    "N_EXP_GEQ",
    "N_EXP_NEQ",
    "N_EXP_EQL",
    "N_EXP_AND",
    "N_EXP_OR",
    "N_EXP_ASSIGN",
    "N_ARG_LIST",
    "N_ARG_LIST_NIL",
    "N_STMT_LABEL_CASE",
    "N_STMT_LABEL_DEFAULT",
    "N_STMT_COMPOUND",
    "N_STMT_EMPTY",
    "N_STMT_EXPRESSION",
    "N_STMT_IF",
    "N_STMT_IF_ELSE",
    "N_STMT_SWITCH",
    "N_STMT_WHILE",
    "N_STMT_DO",
    "N_STMT_FOR",
    "N_STMT_RETURN",
    "N_STMT_CONTINUE",
    "N_STMT_BREAK",
    "N_FOR_EXP",
    "N_STMT_LIST",
    "N_STMT_LIST_NIL",
    "N_INIT_LIST",
    "N_INIT_LIST_ONE",
    "N_INIT_LIST_NIL",
};

char *type_kind_name[] = {
    "NULL",
    "ENUM",
    "ARRAY",
    "STRUCT",
    "UNION",
    "FUNC",
    "POINTER",
    "VOID",
};

char *id_kind_name[] = {
    "NULL",
    "VAR",
    "FUNC",
    "PARM",
    "FIELD",
    "TYPE",
    "ENUM",
    "STRUCT",
    "ENUM_LITERAL",
};
char *spec_name[] = {
    "NULL",
    "AUTO",
    "TYPEDEF",
    "STATIC",
};

void print_node(A_NODE *node, int s)
{
    print_space(s);
    printf("%s (%p,%d)\n", node_name[node->name], node->type, node->value.i);
}

void print_space(int s)
{
    while(s--) printf("| ");
}

void print_ast(A_NODE *node)
{
    printf("======= syntax tree =======\n");
    prt_program(node, 0);
}

void prt_program(A_NODE *node, int s)
{
    print_node(node, s);
    switch(node->name) {
        case N_PROGRAM:
            prt_A_ID_LIST(node->clink, s+1);
            break;
        default:
            printf("***sytax tree error***\n");
    }
}

void prt_initializer(A_NODE *node, int s)
{
    print_node(node, s);
    switch(node->name) {
        case N_INIT_LIST:
            prt_initializer(node->llink, s+1);
            prt_initializer(node->rlink, s+1);
            break;
        case N_INIT_LIST_ONE:
            prt_expression(node->clink, s+1);
            break;
        case N_INIT_LIST_NIL:
            break;
        default:
            printf("***syntax tree error***\n");
    }
}

void prt_expression(A_NODE *node, int s)
{
    print_node(node, s);
    switch(node->name) {
        case N_EXP_IDENT:
            prt_A_ID_NAME(node->clink, s+1);
            break;
        case N_EXP_INT_CONST:
            prt_integer(node->value.i, s+1);
            break;
        case N_EXP_FLOAT_CONST:
            prt_float(node->value.f, s+1);
            break;
        case N_EXP_CHAR_CONST:
            prt_char(node->value.c, s+1);
            break;
        case N_EXP_STRING_LITERAL:
            prt_STRING(node->value.s, s+1);
            break;
        case N_EXP_ARRAY:
            prt_expression(node->llink, s+1);
            prt_expression(node->rlink, s+1);
            break;
        case N_EXP_FUNCTION_CALL:
            prt_expression(node->llink, s+1);
            prt_arg_expr_list(node->rlink, s+1);
            break;
        case N_EXP_STRUCT:
        case N_EXP_ARROW:
            prt_expression(node->llink, s+1);
            prt_STRING(node->rlink, s+1);
            break;
        case N_EXP_POST_INC:
        case N_EXP_POST_DEC:
        case N_EXP_PRE_INC:
        case N_EXP_PRE_DEC:
        case N_EXP_AMP:
        case N_EXP_STAR:
        case N_EXP_NOT:
        case N_EXP_PLUS:
        case N_EXP_MINUS:
        case N_EXP_SIZE_EXP:
            prt_expression(node->clink, s+1);
            break;
        case N_EXP_SIZE_TYPE:
            prt_A_TYPE(node->llink, s+1);
            break;
        case N_EXP_CAST:
            prt_A_TYPE(node->llink, s+1);
            prt_expression(node->rlink, s+1);
            break;
        case N_EXP_MUL:
        case N_EXP_DIV:
        case N_EXP_MOD:
        case N_EXP_ADD:
        case N_EXP_SUB:
        case N_EXP_LSS:
        case N_EXP_GTR:
        case N_EXP_LEQ:
        case N_EXP_GEQ:
        case N_EXP_AND:
        case N_EXP_OR:
        case N_EXP_ASSIGN:
            prt_expression(node->llink, s+1);
            prt_expression(node->rlink, s+1);
            break;
        default:
            printf("**syntax tree error***\n");
    }
}

void prt_arg_expr_list(A_NODE *node, int s)
{
    print_node(node, s);
    switch(node->name) {
        case N_ARG_LIST:
            prt_expression(node->llink, s+1);            
            prt_arg_expr_list(node->rlink, s+1);            
            break;
        case N_ARG_LIST_NIL:
            break;
        default:
            printf("***syntax tree error***\n");
    }
}

void prt_statement(A_NODE *node, int s)
{
    print_node(node, s);

    switch(node->name) {
        case N_STMT_LABEL_CASE:
            prt_expression(node->llink, s+1);
            prt_statement(node->rlink, s+1);
            break;
        case N_STMT_LABEL_DEFAULT:
            prt_statement(node->clink, s+1);
            break;
        case N_STMT_COMPOUND:
            if(node->llink)
                prt_A_ID_LIST(node->llink, s+1);
            prt_statement_list(node->rlink, s+1);
            break;
        case N_STMT_EMPTY:
            break;
        case N_STMT_EXPRESSION:
            prt_expression(node->clink, s+1);
            break;
        case N_STMT_IF_ELSE:
            prt_expression(node->llink, s+1);
            prt_statement(node->clink, s+1);
            prt_statement(node->rlink, s+1);
            break;
        case N_STMT_IF:
        case N_STMT_SWITCH:
            prt_expression(node->llink, s+1);
            prt_statement(node->rlink, s+1);
            break;
        case N_STMT_WHILE:
            prt_expression(node->llink, s+1);
            prt_statement(node->rlink, s+1);
            break;
        case N_STMT_DO:
            prt_statement(node->llink, s+1);
            prt_expression(node->rlink, s+1);
            break;
        case N_STMT_FOR:
            prt_for_expression(node->llink, s+1);
            prt_statement(node->rlink, s+1);
            break;
        case N_STMT_CONTINUE:
            break;
        case N_STMT_BREAK:
            break;
        case N_STMT_RETURN:
            if(node->clink)
                prt_expression(node->clink, s+1);
            break;
        default:
            printf("***syntax tree error***\n");
    }
}
void prt_statement_list(A_NODE *node, int s)
{
    print_node(node, s);
    switch(node->name) {
        case N_STMT_LIST:
            prt_statement(node->llink, s+1);
            prt_statement_list(node->rlink, s+1);
            break;
        case N_STMT_LIST_NIL:
            break;
        default:
            printf("***syntax tree error***\n");
    }
}
void prt_for_expression(A_NODE *node, int s)
{
    print_node(node, s);
    switch(node->name) {
        case N_FOR_EXP:
            if(node->llink)
                prt_expression(node->llink, s+1);
            if(node->clink)
                prt_expression(node->clink, s+1);
            if(node->rlink)
                prt_expression(node->rlink, s+1);
            break;
        default:
            printf("***syntax tree error***\n");
    }
}

void prt_integer(int a, int s)
{
    print_space(s);
    printf("%d\n", a);
}

void prt_float(float f, int s)
{
    print_space(s);
    printf("%f\n", f);
}

void prt_char(char c, int s)
{
    print_space(s);
    printf("%c\n", c);
}

void prt_STRING(char *str, int s)
{
    print_space(s);
    printf("%s\n", str);
}

void prt_A_TYPE(A_TYPE *t, int s)
{
    print_space(s);
    if(t == int_type)
        printf("(int)\n");
    else if(t == float_type) {
        printf("(float)\n");
    } else if(t == char_type) {
        printf("(char %d)\n", t->size);
    } else if(t == void_type) {
        printf("(void)\n");
    } else if(t->kind == T_NULL) {
        printf("(null)\n");
    } else if(t->prt) {
        printf("(DONE: %p)\n", t);
    } else {
        switch(t->kind) {
            case T_NULL:
                break;
            case T_ENUM:
                t->prt = true;
                printf("ENUM\n");
                print_space(s); printf("| ENUMERATORS\n");
                prt_A_ID_LIST(t->field, s+2);
                break;
            case T_POINTER:
                t->prt = true;
                printf("POINTER\n");
                print_space(s); printf("| ELEMENT_TYPE\n");
                prt_A_TYPE(t->element_type, s+2);
                break;
            case T_ARRAY:
                t->prt = true;
                printf("ARRAY\n");
                print_space(s); printf("| INDEX\n");
                if(t->expr)
                    prt_expression(t->expr, s+2);
                else {
                    print_space(s+2);
                    printf("(none)\n");
                }
                print_space(s); printf("| ELEMENT_TYPE\n");
                prt_A_TYPE(t->element_type, s+1);
                break;
            case T_STRUCT:
                t->prt = true;
                printf("STRUCT\n");
                print_space(s); printf(" FIELD\n");
                prt_A_ID_LIST(t->field, s+2);
                break;
            case T_UNION:
                t->prt = true;
                printf("UNION\n");
                print_space(s); printf("| FIEDL\n");
                prt_A_ID_LIST(t->field, s+2);
                break;
            case T_FUNC:
                t->prt = true;
                printf("FUNCTION\n");
                print_space(s); printf("| PARAMETER\n");
                prt_A_ID_LIST(t->field, s+2);
                print_space(s); printf("| TYPE\n");
                prt_A_TYPE(t->element_type, s+2);
                if(t->expr) {
                    print_space(s); printf("| BODY\n");
                    prt_statement(t->expr, s+2);
                }
                break;
            default:
                printf("***syntax tree error***\n");
                break;
        }
    }
}

void prt_A_ID_LIST(A_ID *id, int s)
{
    while(id) {
        prt_A_ID(id, s);
        id = id->link;
    }
}

void prt_A_ID_NAME(A_ID *id, int s)
{
    print_space(s);
    printf("(ID=\"%s\") TYPE: %p KIND: %s SPEC=%s LEV=%d VAL=%d ADDR=%x\n", id->name, id->type, id_kind_name[id->kind], spec_name[id->specifier], id->level, id->value, id->address);
}

void prt_A_ID(A_ID *id, int s)
{
    print_space(s);
    printf("(ID\"%s\") TYPE: %p KIND: %s SPEC=%s LEV=%d VAL=%d ADDR=%x\n", id->name, id->type, id_kind_name[id->kind], spec_name[id->specifier], id->level, id->value, id->address);
    if(id->type) {
        print_space(s);
        printf("| TYPE\n");
        prt_A_TYPE(id->type, s+2);
    } 
    if(id->init) {
        print_space(s);
        printf("| INIT\n");
        if(id->kind == ID_ENUM_LITERAL)
            prt_expression(id->init, s+2);
        else
            prt_initializer(id->init, s+2);
    }
}

void print_tab(int s)
{
    while(s--) {
        printf("  ");
    }
}

void printProgram(A_NODE *node, int s)
{
    printA_ID_LIST(node->clink, s);
}

void printStatement(A_NODE *node, int s)
{
    assert(node != NULL);
    switch (node->name)
    {
    case N_STMT_LABEL_CASE:
        print_tab(s);
        printf("case ");
        printExpression(node->llink);
        printf(" : \n");
        printStatement(node->rlink, s+1);
        break;
    case N_STMT_LABEL_DEFAULT:
        print_tab(s);
        printf("default : \n ");
        printStatement(node->clink, s+1);
        break;
    case N_STMT_COMPOUND:
        print_tab(s);
        printf("{\n ");
        if(node->llink)
            printA_ID_LIST(node->llink, s);
        printStatement_list(node->rlink, s);
        print_tab(s);
        printf("}\n ");
        break;
    case N_STMT_EMPTY:
        print_tab(s);
        printf(";\n ");
        break;
    case N_STMT_EXPRESSION:
        print_tab(s);
        printExpression(node->clink);
        printf(";\n ");
        break;
    case N_STMT_IF_ELSE:
        print_tab(s);
        printf("if ( ");
        printExpression(node->llink);
        printf(")\n");
        printStatement(node->clink, s+1);
        print_tab(s);
        printf("else\n ");
        printStatement(node->rlink, s+1);
        break;
    case N_STMT_IF:
        print_tab(s);
        printf("if (");
        printExpression(node->llink);
        printf(")\n");
        printStatement(node->rlink, s+1);
        break;
    case N_STMT_SWITCH:
        print_tab(s);
        printf("switch (");
        printExpression(node->llink);
        printf(")");
        printStatement(node->rlink, s+1);
        break;
    case N_STMT_WHILE:
        print_tab(s);
        printf("while ( ");
        printExpression(node->llink);
        printf(" )");
        printStatement(node->rlink, s+1);
        break;
    case N_STMT_DO:
        print_tab(s);
        printf("do ");
        printStatement(node->llink, s+1);
        print_tab(s);
        printf("while (");
        printExpression(node->rlink);
        printf(");\n");
        break;
    case N_STMT_FOR:
        print_tab(s);
        printf("for ( ");
        printFor_expression(node->llink);
        printf(" )\n");
        printStatement(node->rlink, s+1);
        break;
    case N_STMT_CONTINUE:
        print_tab(s);
        printf("continue; \n ");
        break;
    case N_STMT_BREAK:
        print_tab(s);
        printf("break; \n ");
        break;
    case N_STMT_RETURN:
        print_tab(s);
        printf("return ");
        if(node->clink)
            printExpression(node->clink);
        printf("; \n ");
        break;
    default:
        printf("***syntax tree error***\n");
        break;
    }
}

void printStatement_list(A_NODE *node, int s)
{
    switch(node->name) {
        case N_STMT_LIST:
            printStatement(node->llink, s);
            printStatement_list(node->rlink, s);
            break;
        case N_STMT_LIST_NIL:
            break;
        default:
            printf("***syntax tree error***");
    }
}

void printExpression(A_NODE *node)
{
    switch(node->name) {
        case N_EXP_IDENT:
            printf("%s", ((A_ID*)(node->clink))->name);
            break;
        case N_EXP_INT_CONST:
            printf("%d", node->value.i);
            break;
        case N_EXP_FLOAT_CONST:
            printf("%f", node->value.f);
            break;
        case N_EXP_CHAR_CONST:
            printf("'%c'", node->value.c);
            break;
        case N_EXP_STRING_LITERAL:
            printf("\"%s\"", node->value.s);
            break;
        case N_EXP_ARRAY:
            printf("(");
            printExpression(node->llink);
            printf(")[");
            printExpression(node->rlink);
            printf("]");
            break;
        case N_EXP_FUNCTION_CALL:
            printf("(");
            printExpression(node->llink);
            printf(")(");
            printArg_expr_list(node->rlink);
            printf(")");
            break;
        case N_EXP_STRUCT:
            printf("(");
            printExpression(node->llink);
            printf(").%s", (char*)node->rlink);
            break;
        case N_EXP_ARROW:
            printf("(");
            printExpression(node->llink);
            printf(")->%s", (char*)node->rlink);
            break;
        case N_EXP_POST_INC:
            printf("(");
            printExpression(node->clink);
            printf(")++");
            break;
        case N_EXP_POST_DEC:
            printf("(");
            printExpression(node->clink);
            printf(")--");
            break;
        case N_EXP_PRE_INC:
            printf("++(");
            printExpression(node->clink);
            printf(")");
            break;
        case N_EXP_PRE_DEC:
            printf("--(");
            printExpression(node->clink);
            printf(")");
            break;
        case N_EXP_AMP:
            printf("&(");
            printExpression(node->clink);
            printf(")");
            break;
        case N_EXP_STAR:
            printf("*(");
            printExpression(node->clink);
            printf(")");
            break;
        case N_EXP_NOT:
            printf("!(");
            printExpression(node->clink);
            printf(")");
            break;
        case N_EXP_PLUS:
            printf("+(");
            printExpression(node->clink);
            printf(")");
            break;
        case N_EXP_MINUS:
            printf("-(");
            printExpression(node->clink);
            printf(")");
            break;
        case N_EXP_SIZE_EXP:
            printf("sizeof (");
            printExpression(node->clink);
            printf(")");
            break;
        case N_EXP_SIZE_TYPE:
            printf("sizeof (");
            printExpression(node->clink);
            printf(")");
            break;
        case N_EXP_CAST:
            printf("(");
            printA_TYPE(node->llink);
            printf(")");
            printf("(");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_MUL:
            printf("(");
            printExpression(node->llink);
            printf(") * (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_DIV:
            printf("(");
            printExpression(node->llink);
            printf(") / (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_MOD:
            printf("(");
            printExpression(node->llink);
            printf(") %% (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_ADD:
            printf("(");
            printExpression(node->llink);
            printf(") + (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_SUB:
            printf("(");
            printExpression(node->llink);
            printf(") - (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_LSS:
            printf("(");
            printExpression(node->llink);
            printf(") < (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_GTR:
            printf("(");
            printExpression(node->llink);
            printf(") > (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_LEQ:
            printf("(");
            printExpression(node->llink);
            printf(") <= (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_GEQ:
            printf("(");
            printExpression(node->llink);
            printf(") >= (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_NEQ:
            printf("(");
            printExpression(node->llink);
            printf(") != (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_EQL:
            printf("(");
            printExpression(node->llink);
            printf(") == (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_AND:
            printf("(");
            printExpression(node->llink);
            printf(") && (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_OR:
            printf("(");
            printExpression(node->llink);
            printf(") || (");
            printExpression(node->rlink);
            printf(")");
            break;
        case N_EXP_ASSIGN:
            printf("(");
            printExpression(node->llink);
            printf(") = (");
            printExpression(node->rlink);
            printf(")");
            break;
        default:
            printf("***syntax tree error***");
            break;
    }
}

void printA_ID_LIST(A_ID *id_list, int s)
{
    A_ID *id = id_list;
    while(id) {
        printA_ID(id, s);
        id = id->link;
    }
}

void printA_ID(A_ID *id, int s)
{
    switch(id->kind) {
        case ID_NULL:
            break;
        case ID_VAR:
            printA_TYPE(id->type);
            printf(" %s", id->name);
            if(id->init) {
                printf(" = ");
                printExpression(id->init);
            }
            printf(";\n");
            break;
        case ID_FUNC:
            printA_TYPE(id->type->element_type);
            printf(" %s", id->name);
            if(id->type->expr)
                printStatement(id->type->expr, s+1);
            break;
        case ID_PARM:
            printf("%s", id->name);
            break;
        case ID_FIELD:
            printf("%s", id->name);
            break;
        case ID_TYPE:
            printf("%s", id->name);
            break;
        case ID_ENUM:
            printf("%s", id->name);
            break;
        case ID_STRUCT:
            break;
            printf("%s", id->name);
        case ID_ENUM_LITERAL:
            printf("%s", id->name);
            break;
    }
}

void printA_TYPE(A_TYPE *type)
{
    A_ID* id = getTypeIdentifier(type);
    if(id == NULL) {
        printf("***syntax tree error***\n");
        return;
    }
    switch(id->kind) {
        case ID_TYPE:
            printf("%s", id->name);
            break;
        case ID_ENUM:
            printf("enum %s", id->name);
            break;
        case ID_STRUCT:
            printf("struct %s", id->name);
            break;
        default:
            printf("***syntax tree error***\n");
            break;
    }
}

void printArg_expr_list(A_NODE *node)
{
    switch(node->name) {
        case N_ARG_LIST:
            printExpression(node->llink);
            if(((A_NODE*)(node->rlink))->name == N_ARG_LIST)
                printf(", ");
            printArg_expr_list(node->rlink);
            break;
        case N_ARG_LIST_NIL:
            break;
        default:
            printf("***syntax tree error***\n");
    }
}

void printIntializer(A_NODE *node)
{

}

void printFor_expression(A_NODE *node)
{
    switch(node->name) {
        case N_FOR_EXP:
            if(node->llink)
                printExpression(node->llink);
            printf("; ");
            if(node->clink)
                printExpression(node->clink);
            printf("; ");
            if(node->rlink)
                printExpression(node->rlink);
            break;
        default:
            printf("***syntax tree error***\n");
            break;
    }
}