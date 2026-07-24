#include "sematic.h"

#include "common.h"
#include "syntax.h"
#include <limits>

int global_address = 12;

A_LITERAL literal_table[LIT_MAX];
int literal_no = 0;
int literal_size = 0;

void semantic_analysis(A_NODE *node)
{
    sem_program(node);
    set_literal_addresss(node);
}


void sem_program(A_NODE *node)
{
    switch(node->name) {
        case N_PROGRAM:
            sem_declaration_list(node->clink, 12);
            node->value = global_address;
            break;
    }
}

int sem_declaration_list(A_ID *id, int addr)
{
    int i = addr;
    while(id) {
        addr += sem_declartion(id, addr);
        id = id->link;
    }
    return (addr-i);
}

int sem_declaration(A_ID *id, int addr)
{
    A_TYPE *t;
    int result = 0, i;
    A_LITERAL lit;
    switch(id->kind) {
        case ID_VAR:
            i = sem_A_TYPE(id->type);
            if(isArrayType(id->type) && id->type->expr == 0)
                sematic_error(86, id->line);
            if(i%4) i = i/4 * 4 + 4;
            if(id->specifier == S_STATIC)
                id->level = 0;
            if(id->level == 0) {
                id->addresss = global_address;
                result = i;
            }
            else {
                id->address = addr;
                result = i;
            }
            if(id->init)
                sem_initializer(id->init, id->level);
            break;
        case ID_FIELD:
            i = sem_A_TYPE(id->type);
            if(isFunctionType(id->type) || isVoidType(id->type))
                sematic_error(84, id->line);
            if(i%4) i = i/4*4+4;
            id->address = addr;
            result = i;
            break;
        case ID_FUNC:
            i = sem_A_TYPE(id->type);
            break;
        case ID_PARM:
            if(id->type) {
                result = sem_A_TYPE(id->type);
                if(id->type == char_type)
                    id->type = int_type;
                else if(isArrayType(id->type)) {
                    id->type->kind = T_POINTER;
                    id->type->size = 4;
                } else if(isFunctionType(id->type)) {
                    t = makeType(T_POINTER);
                    t->element_type = id->type;
                    t->size = 4;
                    id->type = t;
                }
                result = id->type->size;
                if(result%4) result = result/4*4+4;
                break;
            }
        case ID_TYPE:
            i = sem_A_TYPE(id->type);
            break;
    }
    return result;
}

void *sem_initializer(A_NODE *node)
{
    A_TYPE *t;
    switch(node->name) {
        case N_INIT_LIST:
            sem_initializer(node->llink);
            sem_initializer(node->rlink);
            break;
        case N_INIT_LIST_ONE:
            sem_initializer(node->clink);
            break;
        case N_INIT_LIST_NIL:
            break;
    }
}

int sem_A_TYPE(A_TYPE *t)
{
    A_ID *id;
    A_LITERAL lit;
    int result = 0, i;
    if(t->check)
        return t->size;
    t->check = 1;
    switch(t->kind) {
        case T_ENUM:
            i = 0;
            id = t->field;
            while(id) {
                if(id->init) {
                    lit = getTypeAndValueOfExpression(id->init);
                    if(!isIntType(lit.type))
                        sematic_error(81, id->line);
                    i = lit.value.i;
                }
                id->init = i++;
                id = id->link;
            }
            result = 4;
            break;
        case T_ARRAY:
            if(t->expr) {
                lit = getTypeAndValueOfExpression(t->expr);
                if(!isIntType(lit.type) || lit.value <= 0) {
                    semantic_error(82, t->line);
                    t->expr = 0;
                }
                else {
                    t->expr = lit.value.i;
                }
            }
            result = sem_A_TYPE(t->element_type)*(int)t->expr;
            if(isVoidType(t->element_type) || isFunctionType(t->element_type))
                semantic_error(83, t->line);
            break;
        case T_STRUCT:
            result = 0;
            id = t->field;
            while(id) {
                result += sem_declaration(id, result);
                id = id->link;
            }
            break;
        case T_UNION:
            result = 0;
            id = t->field;
            while(id) {
                i = sem_declaration(id, 0);
                if(i>result)
                    result = i;
                id = id->link;
            }
            break;
        case T_FUNC:
            tt = t->element_type;
            i = sem_A_TYPE(tt);
            if(isArrayType(tt) || isFunctionType(tt))
                semantic_error(85, t->line);
            i = sem_declartion(t->field, 12) + 12;
            if(t->expr) {
                i = i + sem_statement(t->expr, i, t->element_type, false, false, false);
            }
            t->local_var_size = i;
            break;
        case T_POINTER:
            i = sem_A_TYPE(t->element_type);
            result = 4;
            break;
        case T_VOID:
            break;
    }
    t->size = result;
    return result;
}

A_TYPE *sem_expression(A_NODE *node)
{
    A_TYPE *result = NIL, *t, *t1, *t2;
    A_ID *id;
    A_LITERAL lit;
    int lvalue = 0;
    switch(node->name) {
        case N_EXP_IDENT:
            id = node->llink;
            switch(id->kind) {
                case ID_VAR:
                case ID_PARM:
                    result = id->type;
                    if(!isArrayType(result))
                        lvalue = 1;
                    break;
                case ID_FUNC:
                    result = id->type;
                    break;
                case ID_ENUM_LITERAL:
                    result = int_type;
                    break;
            }
            break;
        case N_EXP_INT_CONST:
            result = int_type;
            break;
        case N_EXP_FLOAT_CONST:
            lit.type = float_type; lit.value.f = atof(node->llink);
            node->llink = put_literal(lit, node->line);
            result = float_type;
            break;
        case N_EXP_CHAR_CONST:
            result = char_type;
            break;
        case N_EXP_STRING_LITERAL:
            lit.type = string_type; lit.value.s = node->llink;
            node->llink = put_literal(lit, node->line);
            result = string_type;
            break;
        case N_EXP_ARRAY:
            t1 = sem_expression(node->llink);
            t2 = sem_expression(node->rlink);
            t = convertUsualBinaryConversion(node);
            t1 = node->llink->type;
            t2 = node->rlink->type;
            if(isPointerOrArrayType(t1))
                result = t1->element_type;
            else
                sematic_error(32, node->line);
            if(!isIntegralType(t2))
                semantic_error(29, node->line);
            if(!isArrayType(result))
                lvalue = true;
            break;
        case N_EXP_STRUCT:
            t = sem_expression(node->llink);
            id = getStructFieldIdentifier(t, node->rlink);
            if(id) {
                result = id->type;
                if(node->llink_.vluae && !isArrayType(result))
                    lvalue = true;
            }
            else
                semantic_error(37, node->line);
            node->rlink = id;
            break;
        case N_EXP_ARROW:
            t = sem_expression(node->llink);
            id = getPointerFieldIdentifier(t, node->rlink);
            if(id) {
                result = id->type;
                if(!isArrayType(result))
                    lvalue = true;
            }
            else
                semantic_error(37, node->line);
            node->rlink = id;
            break;
        case N_EXP_FUNCTION_CALL:
            t = sem_expression(node->llink);
            node->llink = convertUsualUnaryConverion(node->llink);
            t = node->llink->type;
            if(isPointerType(t) && isFunctionType(t->element_type)) {
                sem_arg_expr_list(node->rlink, t->element_type->field);
                result = t->element_type->element_type;
            }
            else
                semantic_error(21, node->line);
            break;
        case N_EXP_POST_INC:
        case N_EXP_POST_INC:
            result = sem_expression(node->clink);
            if(!isScalarType(result))
                semantic_error(27, node->line);
            if(!isModifiableLvalue(node->clink))
                semantic_error(60, node->line);
            break;
            case N_EXP_CAST:
                result = node->llink;
                i = sem_A_TYPE(result);
                t = sem_expression(node->rlink);
                if(!isAllowableCastingConversion(result, t))
                    semantic_error(58, node->line);
                break;
            case N_EXP_SIZE_EXP:
                t = node->clink;
                i = sem_A_TYPE(t);
                if(isArrayType(t) && t->size == 0 || isFunctionType(t) || isVoidType(t))
                    semantic_error(39, node->line);
                else
                    node->clink = i;
                result = int_type;
                break;
            case N_EXP_PLUS:
            case N_EXP_MINUS:
                t = sem_expression(node->clink);
                if(isArithmeticType(t)) {
                    node->clink = convertUsualUnaryConverion(node->clink);
                    result = node->clink->type;
                }
                else
                    semantic_error(13, node->line);
                break;
            case N_EXP_NOT:
                t = sem_expression(node->clink);
                if(isScalarType(t)) {
                    node->clink = convertUsualUnaryConversion(node->clink);
                    result = int_type;
                }
                else
                    semantic_error(27, node->line);
                break;
            case N_EXP_AMP:
                t = sem_expression(node->clink);
                if(node->clink->value == true || isFunctionType(t)) {
                    result = setTypeElementType(makeType(T_POINTER), t);
                    result->size = 4;
                }
                else
                    semantic_error(60, node->line);
                break;
            case N_EXP_STAR:
                t = sem_expression(node->clink);
                node->clink = convertUsualUnaryConverion(node->clink);
                if(isPointerType(t)) {
                    result = t->element_type;
                    if(isStructOrUnionType(result) || isScalarType(result))
                        lvalue = true;
                }
                else
                    semantic_error(31, node->line);
                break;
            case N_EXP_PRE_INC:
            case N_EXP_PRE_DEC:
                result = sem_expression(node->clink);
                if(!isScalarType(result))
                    semantic_error(27, node->line);
                if(!isModifiableLvalue(node->clink))
                    semantic_error(60, node->line);
                break;
            case N_EXP_MUL:
            case N_EXP_DIV:
                t1 = sem_expression(node->llink);
                t2 = sem_expression(node->rlink);
                if(isArithmeticType(t1) && isArithmeticType(t2))
                    result = convertUsualBinaryConversion(node);
                else
                    semantic_error(28, node->line);
                break;
            case N_EXP_MOD:
                t1 = sem_expression(node->llink);
                t2 = sem_expression(node->rlink);
                if(isInteralType(t1) && isIntegralType(t2))
                    result = convertUsualBinaryConversion(node);
                else
                    semantic_error(29, node->line);
                result = int_type;
                break;
            case N_EXP_ADD:
                t1 = sem_expression(node->llink);
                t2 = sem_expression(node->rlink);
                if(isArithmeticType(t1) && isArithmeticType(t2))
                    result = convertUsualBinaryConversion(node);
                else if(isPointerType(t1) && isIntegralType(t2))
                    result = t1;
                else if(isIntegralType(t1) && isPointerType(t2))
                    result = t2;
                else
                    semantic_error(24, node->line);
                break;
            case N_EXP_SUB:
                t1 = sem_expression(node->llink);
                t2 = sem_expression(node->rlink);
                if(isArithmeticType(t1) && isArithmeticType(t2))
                    result = convertUsualBinaryConversion(node);
                else if(isPointerType(t1) &&  isIntegraltype(t2))
                    result = t1;
                else if(isCompatibleType(t1, t2))
                    result = t1;
                else
                    semantic_error(24, node->line);
                break;
            case N_EXP_LSS:
            case N_EXP_GTR:
            case N_EXP_LEQ:
            case N_EXP_GEQ:
                t1 = sem_expression(node->llink);
                t2 = sem_expression(node->rlink);
                if(isArithmeticType(t1) && isArithmeticType(t2))
                    t = convertUsualBinaryConversion(node);
                else if(!isCompatiblePointerType(t1, t2))
                    semantic_error(40, node->line);
                result = int_type;
                break;
            case N_EXP_NEQ:
            case N_EXP_EQL:
                t1 = sem_expression(node->llink);
                t2 = sem_expression(node->rlink);
                if(isArithmeticType(t1) && isArithmeticType(t2))
                    t = convertUsualBinaryConversion(node);
                else if(!isCompatiblePointerType(t1, t2) &&
                        (!isPointerType(t1) || !isConstantZeroExp(node->rlink)) &&
                        (!isPointerType(t2) || !isConstantZeroExp(node->llink)))
                    semantic_error(40, node->line);
                result = int_type;
                break;
            case N_EXP_AND:
            case N_EXP_OR:
                t = sem_expression(node->llink);
                if(isScalarType(t))
                    node->llink = convertUsualUnaryConverion(node->llink);
                else
                    semantic_error(27, node->line);
                t = sem_expression(node->rlink);
                if(isScalarType(t))
                    node->rlink = convertUsualUnaryConverion(node->rlink);
                else
                    semantic_error(27, node->line);
                result = int_type;
                break;
            case N_EXP_ASSIGN:
                result = sem_expression(node->llink);
                if(!isModifiableLvalue(node->llink))
                    semantic_error(60, node->line);
                t = sem_expression(node->rlink);
                if(isAllowableAssignmentConversion(result, t, node)) {
                    if(isArithmeticType(result) && isArithmeticType(t))
                        node->rlink = convertUsualAssignmentConversion(result, node->rlink);
                }
                else
                    semantic_error(58, node->line);
                break;
    }
    node->type = result;
    node->value = value;
    return result;
}

void sem_arg_expression_list(A_NODE *node, A_ID *id)
{
    A_TYPE *t;
    A_ID *a;
    switch(node->name) {
        case N_ARG_LIST:
            t = sem_expression(node->llink);
            if(id == 0)
                semantic_error(34, node->line);
            else {
                if(id->type) {
                    t = sem_expression(node->llink);
                    node->llink = convertUsualUnaryConverion(node->llink);
                    if(isAllowableCastingConversion(id->type, node->llink->type))
                        node->llink = convertCastingConversion  (node->link, id->type);
                    else
                        semantic_error(59, node->line);
                    sem_arg_expr_list(node->rlink, id->link);
                }
                else {
                    t = sem_expression(node->llink);
                    sem_arg_expr_list(node->rlink, id);
                }
            }
            break;
        case N_ARG_LIST_NIL:
            if(id && id->type)
                semantic_error(35, node->line);
            break;
    }
}

int sem_statement(A_NODE *node, int addr, A_TYPE &*ret, bool sw, bool brk, bool cnt)
{
    int local_size = 0, i;
    A_LITERAL lit;
    A_TYPE *t;
    switch(node->name) {
        case N_STMT_LABEL_CASE:
            if(sw == false)
                semantic_error(71, node->line);
            lit = getTypeAndValueOfExpression(node->llink);
            if(isIntegralType(lit.type))
                node->llinke = lit.value.i;
            else
                semantic_error(51, node->line);
            local_size = sem_statement(node->rlink, addr);
            break;
        case N_STMT_LABEL_DEFAULT:
            if(sw == false)
                semantic_error(72, node->line);
            local_size = sem_statement(node->clink, addr);
            break;
        case N_STMT_COMPOUND:
            if(node->llink)
                local_size = sem_declaration_list(node->llink, addr);
            local_size += sem_statement_list(node->rlink, local_size + addr, ret, sw, brk, cnt);
            break;
        case N_STMT_EMPTY:
            break;
        case N_STMT_EXPRESSION:
            sem_expression(node->clink);
            break;
        case N_STMT_IF:
            t = sem_expression(node->llink);
            if(isScalarType(t))
                node->llink = convertScalarTOInteger(node->llink);
            else
                semantic_error(50, node->line);
            local_size = sem_statement(node->rlink, addr, ret, false, brk, cnt);
        case N_STMT_IF_ELSE:
            t = sem_expression(node->llink);
            if(isScalarType(t))
                node->llink = convgertScalarToInterger(node->llink);
            else
                semantic_error(50, node->line);
            local_isze = sem_statement(node->clink, addr, ret, false, brk, cnt);
            i = sem_statement(node->rlink, addr, ret, false, brk, cnt);
            if(local_size < i)
                local_size = i;
            break;
        case N_STMT_SWITCH:
            t = sem_expression(node->llink);
            if(!isIntegralType(t))
                semantic_error(50, node->line);
            local_size = sem_statement(node->rlink, addr, ret, true, true, cnt);
            break;
        case N_STMT_WHILE:
            t = sem_expression(node->llink);
            if(isScalarType(t))
                node->llink = convertScalarToInteger(node->llink);
            else
                semantic_error(50, node->line);
            local_size = sem_statement(node->rlink, addr, ret, false, true, true);
            break;
        case N_STMT_DO:
            local_size = sem_statement(node->llink, addr, ret, false, true, true);
            t = sem_expression(node->rlink);
            if(isScalarType(t))
                node->rlink = convertScalarToInteger(node->rlink);
            else
                semantic_error(50, node->line);
            break;
        case N_STMT_FOR:
            sem_for_expression(node->llink);
            local_size = sem_statement(node->rlink, addr, ret, false, true, true);
            break;
        case N_STMT_CONTINUE:
            if(cnt == false)
                semantic_error(74, node->line);
            break;
        case N_STMT_BREAK:
            if(brk == false)
                semantic_error(73, node->line);
            break;
        case N_STMT_RETURN:
            if(node->clink) {
                t = sem_expression(node->clink);
                if(isAllowableCastingConversion(ret, t))
                    node->clink = convertCastingConversion(node->clink, ret);
                else
                 semantic_error(59, node->line);
            }
            break;
    }
    node->value = local_size;
    return local_size;
}

int sem_statement_list(A_NODE *node, int addr, A_TYPE *ret, bool sw, bool brk, bool cnt)
{
    int size, i;
    switch(node->name) {
        case N_STMT_LIST:
            size = sem_statement(node->llink, addr, ret, sw, brk, cnt);
            i = sem_statement_list(node->rlink, addr, ret, sw, brk, cnt);
            if(size < i)
                size = i;
            break;
        case N_STMT_LIST_NIL:
            size = 0;
            break;
    }
    node->value = size;
    return size;
}

void sem_for_expression(A_NODE *node)
{
    A_TYPE *t;
    switch(node->name) {
        case N_FOR_EXP:
            if(node->llink)
                t = sem_expression(node->llink);
            if(node->clink) {
                t = sem_expression(node->clink);
                if(isScalarType(t))
                    node->clink = convertScalarToInteger(node->clink);
                else
                    semantic_error(28, node->line);
            }
            if(node->rlink)
                t = sem_expression(node->rlink);
    }
}


void put_literal(A_LITERAL lit, int ll)
{
    float ff;
    if(literal_no >= LIT_MAX)
        semantic_error(93, ll);
    else
        literal_no++;
    literal_table[literal_no] = lit;
    literal_table[literal_no].addr = literal_size;
    if(lit.type->kind == T_ENUM)
        literal_size += 4;
    else if(lit.type == string_type)
        literal_size += strlen(lit.value.s) + 1;
    if(literal_size%4)
        literal_size = literal_size / 4 * 4 + 4;
    return literal_no;
}

A_LITERAL getTypeAndValueOfExpression(A_NODE *node)
{
    A_TYPE *t;
    A_ID *id;
    A_LITERAL result, r;
    result.type = NIL;
    switch(node->name) {
        case N_EXP_IDENT:
            id = node->clink;
            if(id->kind != ID_ENUM_LITERAL)
                semantic_error(19, node->line, id->name);
            else {
                result.type = int_type;
                result.value.i = id->value;
            }
            break;
        case N_EXP_INT_CONST:
            result.type = int_type;
            result.value.i = (int)node->clink;
            break;
        case N_EXP_CHAR_CONST:
            result.type = char_type;
            result.value.c = (char)node->clink;
            break;
        case N_EXP_FLOAT_CONST:
            result.type = float_type;
            result.value.f = atof(node->clink);
            break;
        case N_EXP_STRING_LITERAL:
        case N_EXPR_ARRAY:
        case N_EXP_FUNCTION_CALL:
        case N_EXP_STRUCT:
        case N_EXP_ARROW:
        case N_EXP_POST_INC:
        case N_EXP_PRE_INC:
        case N_EXP_POST_DEC:
        case N_EXP_PRE_DEC:
        case N_EXP_AMP:
        case N_EXP_STAR:
        case N_EXP_NOT:
            semantic_error(18, node->line);
            break;
        case N_EXP_MINUS:
            result = getTypeAndValueOfExpression(node->clink);
            if(result.type == int_type)
                result.value.i = -result.value.i;
            else if(result.type == float_type)
                result.value.f = -result.value.f;
            else
                semantic_error(18, node->line);
            break;
        case N_EXP_SIZE_EXP:
            t = sem_expression(node->clink);
            result.type = int_type;
            result.value.i = t->size;
            break;
        case N_EXP_SIZE_TYPE:
            result.type = int_type;
            result.value.i = sem_A_TYPE(node->clink);
            break;
        case N_EXP_CAST:
            result = getTypeAndValueOfExpression(node->rlink);
            result = checkTypeAndConvertLiteral(result, (A_TYPE *)node->llink, node->line);
            break;
        case N_EXP_MUL:
            result = getTypeAndValueOfExpression(node->llink);
            r = getTypeAndValueOfExpression(node->rlink);
            if(result.type == int_type && r.type == int_type) {
                result.type = int_type;
                result.value.i = result.value.i * r.value.i;
            }
            else if(result.type == int_type && r.type == float_type) {
                result.type = float_type;
                result.value.f = result.value.i * r.value.f;
            }
            else if(result.type == float_type && r.type == int_type) {
                result.type = float_type;
                result.value.f = result.value.f * r.value.i;
            }
            else if(result.type == float_type && r.type == float_type) {
                result.type == float_type;
                result.value.f = result.value.f * r.value.f;
            }
            else
                semantic_error(18, node->line);
            break;
        case N_EXP_DIV:
            result = getTypeAndValueOfExpression(node->llink);
            r = getTypeAndValueOfExpression(node->rlink);
            if(result.type == int_type && r.type == int_type) {
                result.type = int_type;
                result.value.i = result.value.i / r.value.i;
            }
            else if(result.type == int_type && r.type == float_type) {
                result.type = float_type;
                result.value.f = result.value.i / r.value.f;
            }
            else if(result.type == float_type && r.type == int_type) {
                result.type = float_type;
                result.value.f = result.value.f / r.value.i;
            }
            else if(result.type == float_type && r.type == float_type) {
                result.type == float_type;
                result.value.f = result.value.f / r.value.f;
            }
            else
                semantic_error(18, node->line);
            break;
        case N_EXP_MOD:
            result = getTypeAndValueOfExpression(node->llink);
            r = getTypeAndValueOfExpression(node->rlink);
            if(result.type == int_type && r.type == int_type) {
                result.type = int_type;
                result.value.i = result.value.i / r.value.i;
            }
            else
                semantic_error(18, node->line);
            break;
        case N_EXP_ADD:
            result = getTypeAndValueOfExpression(node->llink);
            r = getTypeAndValueOfExpression(node->rlink);
            if(result.type == int_type && r.type == int_type) {
                result.type = int_type;
                result.value.i = result.value.i + r.value.i;
            }
            else if(result.type == int_type && r.type == float_type) {
                result.type = float_type;
                result.value.f = result.value.i + r.value.f;
            }
            else if(result.type == float_type && r.type == int_type) {
                result.type = float_type;
                result.value.f = result.value.f + r.value.i;
            }
            else if(result.type == float_type && r.type == float_type) {
                result.type == float_type;
                result.value.f = result.value.f + r.value.f;
            }
            else
                semantic_error(18, node->line);
            break;
        case N_EXP_SUB:
            result = getTypeAndValueOfExpression(node->llink);
            r = getTypeAndValueOfExpression(node->rlink);
            if(result.type == int_type && r.type == int_type) {
                result.type = int_type;
                result.value.i = result.value.i - r.value.i;
            }
            else if(result.type == int_type && r.type == float_type) {
                result.type = float_type;
                result.value.f = result.value.i - r.value.f;
            }
            else if(result.type == float_type && r.type == int_type) {
                result.type = float_type;
                result.value.f = result.value.f - r.value.i;
            }
            else if(result.type == float_type && r.type == float_type) {
                result.type == float_type;
                result.value.f = result.value.f - r.value.f;
            }
            else
                semantic_error(18, node->line);
            break;
        case N_EXP_LSS:
        case N_EXP_LEQ:
        case N_EXP_GTR:
        case N_EXP_GEQ:
        case N_EXP_NEQ:
        case N_EXP_EQL:
        case N_EXP_AND:
        case N_EXP_OR:
        case N_EXP_ASSIGN:
            semantic_error(18, node->line);
            break;
    }
}
