#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax.h"

#include "common.h"
#include "type.h"

extern char *yytext;

A_TYPE *int_type, *char_type, *void_type, *float_type, *string_type;
A_NODE *root = NULL;
A_ID *current_id = NULL;
int line_no = 1;
int current_level = 0;
int syntax_err = 0;

A_NODE *makeNode(NODE_NAME name, void *a, void *b, void *c)
{
    A_NODE* m;
    m = (A_NODE*)malloc(sizeof(A_NODE));
    m->name = name;
    m->llink = a;
    m->clink = b;
    m->rlink = c;
    m->type = NULL;
    m->line = line_no;
    memset(&m->value, 0, sizeof(m->value));
    return m;
}

A_NODE *makeNodeList(NODE_NAME name, A_NODE *a, A_NODE *b)
{
    A_NODE *m, *k;
    k = a;
    while(k->rlink)
        k = k->rlink;
    m = (A_NODE*)malloc(sizeof(A_NODE));
    m->name = k->name;
    m->llink = NULL;
    m->clink = NULL;
    m->rlink = NULL;
    m->type = NULL;
    m->line = line_no;
    memset(&m->value, 0, sizeof(m->value));
    k->name = name;
    k->llink = b;
    k->rlink = m;
    return a;
}

A_ID *makeIdentifier(char *s)
{
    A_ID *id;
    id = malloc(sizeof(A_ID));
    id->name = s;
    id->kind = ID_NULL;
    id->specifier = S_NULL;
    id->level = current_level;
    id->address = 0; // TODO
    id->init = NULL;
    id->type = NULL;
    id->link = NULL;
    id->line = line_no;
    id->value = 0;
    id->prev = current_id;
    current_id = id;
    return id;
}

A_ID *makeDummyIdentifier()
{
    A_ID *id;
    id = malloc(sizeof(A_ID));
    id->name = "";
    id->kind = ID_NULL;
    id->specifier = S_NULL;
    id->level = current_level;
    id->address = 0;
    id->init = NULL;
    id->type = NULL;
    id->link = NULL;
    id->line = line_no;
    id->value = 0;
    id->prev = NULL;
    return id;
}

A_TYPE *makeType(T_KIND k)
{
    A_TYPE *t;
    t = (A_TYPE*)malloc(sizeof(A_TYPE));
    t->kind = k;
    t->size = 0;
    t->local_var_size = 0;
    t->element_type = NULL;
    t->field = NULL;
    t->expr = NULL;
    t->check = false;
    t->prt = false;
    t->line = line_no;
    return t;
}

A_SPECIFIER *makeSpecifier(A_TYPE *t, S_KIND s)
{
    A_SPECIFIER *p;
    p = malloc(sizeof(A_SPECIFIER));
    p->type = t;
    p->stor = s;
    p->line = line_no;
    return p;
}

A_ID *searchIdentifier(char *s, A_ID *id)
{
    while(id) {
        if(strcmp(id->name, s) == 0)
            break;
        id = id->prev;
    }
    return id;
}

A_ID *searchIdentifierAtCurrentLevel(char *s, A_ID *id)
{
    while(id) {
        if(id->level < current_level)
            return NULL;
        if(strcmp(id->name, s) == 0)
            break;
        id = id->prev;
    }
    return id;
}

void checkForwardReference()
{
    A_ID *id;
    A_TYPE *t;
    id = current_id;
    while(id) {
        if(id->level < current_level)
            break;
        t = id->type;
        if(id->kind == ID_NULL)
            syntax_error(31, id->name);
        else if((id->kind == ID_STRUCT || id->kind == ID_ENUM) && t->field == NULL)
            syntax_error(32, id->name);
        id = id->prev;
    }
}

void setDefaultSpecifier(A_SPECIFIER *p)
{
    if(p->type == NULL)
        p->type = int_type;
    if(p->stor == S_NULL)
        p->stor = S_AUTO;
}

A_SPECIFIER *updateSpecifier(A_SPECIFIER *p, A_TYPE *t, S_KIND s)
{
    if(t) {
        if(p->type) {
            if(p->type != t)
                syntax_error(24);
        }
        else
            p->type = t;
    }
    if(s) {
        if(p->stor) {
            if(s != p->stor)
                syntax_error(24);
        }
        else
            p->stor = s;
    }
    return p;
}

A_ID *linkDeclaratorList(A_ID *id1, A_ID *id2)
{
    A_ID *m = id1;
    if(id1 == NULL)
        return id2;
    while(m->link)
        m = m->link;
    m->link = id2;
    return id1;
}

A_ID *getIdentifierDeclared(char *s)
{
    A_ID *id;
    id = searchIdentifier(s, current_id);
    if(id == NULL) syntax_error(13, s);
    return id;
}

A_ID *getTypeIdentifier(A_TYPE* t)
{
    A_ID *id;
    id = current_id;
    while(id) {
        if(id->type == t) {
            return id;
        }
        id = id->prev;
    }
    return NULL;
}

A_TYPE *getTypeOfStructOrEnumRefIdentifier(T_KIND k, char *s, ID_KIND kk)
{
    A_TYPE *t;
    A_ID *id;
    id = searchIdentifier(s, current_id);
    if(id) {
        if(id->kind == kk && id->type->kind == k)
            return id->type;
        else
            syntax_error(11, s);
    }
    t = makeType(k);
    id = makeIdentifier(s);
    id->kind = kk;
    id->type = t;
    return t;
}

A_NODE *setNodeInteger(A_NODE* node, int i)
{
    node->value.i = i;
    return node;
}

A_NODE *setNodeFloat(A_NODE* node, float f)
{
    node->value.f = f;
    return node;
}

A_NODE *setNodeChar(A_NODE* node, char c)
{
    node->value.c = c;
    return node;
}

A_NODE *setNodeString(A_NODE* node, char *s)
{
    node->value.s = s;
    return node;
}

A_ID *setDeclaratorInit(A_ID *id, A_NODE *node)
{
    id->init = node;
    return id;
}

A_ID *setDeclaratorKind(A_ID *id, ID_KIND k)
{
    A_ID *a;
    a = searchIdentifierAtCurrentLevel(id->name, id->prev);
    if(a)
        syntax_error(12, id->name);
    id->kind = k;
    return id;
}

A_ID *setDeclaratorType(A_ID *id, A_TYPE *t)
{
    id->type = t;
    return id;
}

A_ID *setDeclaratorElementType(A_ID *id, A_TYPE *t)
{
    A_TYPE *tt;
    if(id->type == NULL)
        id->type = t;
    else {
        tt = id->type;
        while(tt->element_type)
            tt = tt->element_type;
        tt->element_type = t;
    }
    return id;
}

A_ID *setDeclaratorTypeAndKind(A_ID *id, A_TYPE *t, ID_KIND k)
{
    id = setDeclaratorElementType(id, t);
    id = setDeclaratorKind(id, k);
    return id;
}

A_ID *setFunctionDeclaratorSpecifier(A_ID *id, A_SPECIFIER *p)
{
    A_ID *a;
    if(p->stor)
        syntax_error(25);
    setDefaultSpecifier(p);
    if(id->type->kind != T_FUNC) {
        syntax_error(21);
        return id;
    } else {
        id = setDeclaratorElementType(id, p->type);
        id->kind = ID_FUNC;
    }
    a = searchIdentifierAtCurrentLevel(id->name, id->prev);
    if(a) {
        if(a->kind != ID_FUNC || a->type->expr)
            syntax_error(12, id->name);
        else {
            if(isNotSameFormalParameters(a->type->field, id->type->field))
                syntax_error(22, id->name);
            if(isNotSameType(a->type->element_type, id->type->element_type))
                syntax_error(26, a->name);
        }
    }
    a = id->type->field;
    while(a) {
        if(strlen(a->name))
            current_id = a;
        else if(a->type)
            syntax_error(23);
        a = a->link;
    }
    return id;
}

A_ID *setFunctionDeclaratorBody(A_ID *id, A_NODE *n)
{
    id->type->expr = n;
    return id;
}

A_ID *setDeclaratorListSpecifier(A_ID *id, A_SPECIFIER *p)
{
    A_ID *a;
    setDefaultSpecifier(p);
    a = id;
    while(a) {
        if(strlen(a->name) && searchIdentifierAtCurrentLevel(a->name, a->prev))
            syntax_error(12, a->name);
        a = setDeclaratorElementType(a, p->type);
        if(p->stor == S_TYPEDEF)
            a->kind = ID_TYPE;
        else if(a->type->kind == T_FUNC)
            a->kind = ID_FUNC;
        else
            a->kind = ID_VAR;
        a->specifier = p->stor;
        if(a->specifier == S_NULL)
            a->specifier = S_AUTO;
        a = a->link;
    }
    return id;
}

A_ID *setParameterDeclaratorSpecifier(A_ID *id, A_SPECIFIER *p)
{
    if(searchIdentifierAtCurrentLevel(id->name, id->prev))
        syntax_error(12, id->name);
    if(p->stor || p->type == void_type)
        syntax_error(14);
    setDefaultSpecifier(p);
    id = setDeclaratorElementType(id, p->type);
    id->kind = ID_PARM;
    return id;
}

A_ID *setStructDeclaratorListSpecifier(A_ID *id, A_TYPE *t)
{
    A_ID *a;
    a = id;
    while(a) {
        if(searchIdentifierAtCurrentLevel(a->name, a->prev))
            syntax_error(12, a->name);
        a = setDeclaratorElementType(a, t);
        a->kind = ID_FIELD;
        a = a->link;
    }
    return id;
}

A_TYPE *setTypeNameSpecifier(A_TYPE *t, A_SPECIFIER *p)
{
    if(p->stor)
        syntax_error(20);
    setDefaultSpecifier(p);
    t = setTypeElementType(t, p->type);
    return t;
}

A_TYPE *setTypeElementType(A_TYPE *t, A_TYPE *s)
{
    A_TYPE *q;
    if(t == NULL)
        return s;
    q = t;
    while(q->element_type)
        q = q->element_type;
    q->element_type = s;
    return t;
}

A_TYPE *setTypeField(A_TYPE *t, A_ID *n)
{
    t->field = n;
    return t;
}

A_TYPE *setTypeExpr(A_TYPE *t, A_NODE *n)
{
    t->expr = n;
    return t;
}

A_TYPE *setTypeStructOrEnumIdentifier(T_KIND k, char *s, ID_KIND kk)
{
    A_TYPE *t;
    A_ID *id, *a;
    a = searchIdentifierAtCurrentLevel(s, current_id);
    if(a) {
        if(a->kind == kk && a->type->kind == k) {
            if(a->type->field)
                syntax_error(12, s);
            else
                return a->type;
        }
        else
            syntax_error(12, s);
    }
    id = makeIdentifier(s);
    t = makeType(k);
    id->type = t;
    id->kind = kk;
    return t;
}

A_TYPE *setTypeAndKindOfDeclarator(A_TYPE *t, ID_KIND k, A_ID *id)
{
    if(searchIdentifierAtCurrentLevel(id->name, id->prev))
        syntax_error(12, id->name);
    id->type = t;
    id->kind = k;
    return t;
}

bool isNotSameFormalParameters(A_ID *a, A_ID *b)
{
    if(a == NULL)
        return false;
    while(a) {
        if(b == NULL || isNotSameType(a->type, b->type))
            return true;
        a = a->link;
        b = b->link;
    }
    if(b)
        return true;
    else
        return false;
}

bool isNotSameType(A_TYPE *t1, A_TYPE *t2)
{
    if(isPointerOrArrayType(t1) || isPointerOrArrayType(t2))
        return isNotSameType(t1->element_type, t2->element_type);
    else
        return t1 != t2;
}

bool isPointerOrArrayType(A_TYPE *t)
{
    return t->kind == T_ARRAY || t->kind == T_POINTER;
}

void initialize()
{
    int_type = setTypeAndKindOfDeclarator(
        makeType(T_ENUM), ID_TYPE, makeIdentifier("int")
    );
    float_type = setTypeAndKindOfDeclarator(
        makeType(T_ENUM), ID_TYPE, makeIdentifier("float")
    );
    char_type = setTypeAndKindOfDeclarator(
        makeType(T_ENUM), ID_TYPE, makeIdentifier("char")
    );
    void_type = setTypeAndKindOfDeclarator(
        makeType(T_VOID), ID_TYPE, makeIdentifier("void")
    );
    string_type = setTypeElementType(makeType(T_POINTER), char_type);
    int_type->size = 4;
    int_type->check = true;
    float_type->size = 4;
    float_type->check = true;
    char_type->size = 1;
    char_type->check = true;
    void_type->size = 0;
    void_type->check = true;
    string_type->size = 4; // TODO
    string_type->check = true;

    setDeclaratorTypeAndKind(
        makeIdentifier("printf"),
        setTypeField(
            setTypeElementType(makeType(T_FUNC), void_type),
            linkDeclaratorList(
                setDeclaratorTypeAndKind(makeDummyIdentifier(), string_type, ID_PARM),
                setDeclaratorKind(makeDummyIdentifier(), ID_PARM)
            )
        ),
        ID_FUNC
    );
    setDeclaratorTypeAndKind(
        makeIdentifier("scanf"),
        setTypeField(
            setTypeElementType(makeType(T_FUNC), void_type),
            linkDeclaratorList(
                setDeclaratorTypeAndKind(makeDummyIdentifier(), string_type, ID_PARM),
                setDeclaratorKind(makeDummyIdentifier(), ID_PARM)
            )
        ),
        ID_FUNC
    );
    setDeclaratorTypeAndKind(
        makeIdentifier("malloc"),
        setTypeField(
            setTypeElementType(makeType(T_FUNC), string_type),
            setDeclaratorTypeAndKind(makeDummyIdentifier(), int_type, ID_PARM)
        ),
        ID_FUNC
    );
}

void syntax_error(int i, char *s)
{
    syntax_err++;
    printf("line %d: syntax error: ", line_no);
    switch(i) {
        case 11: printf("illegal referencing struct or union identifier %s", s);
                 break;
        case 12: printf("redeclartion of identifier %s", s);
                 break;
        case 13: printf("undifined identifier %s", s);
                 break;
        case 14: printf("illegal type specifier in formal parameter");
                 break;
        case 20: printf("illegal storage class in type specifiers");
                 break;
        case 21: printf("illegal function declarator");
                 break;
        case 22: printf("conflicting parm type in prototype function %s", s);
                 break;
        case 23: printf("empty parameter name");
                 break;
        case 24: printf("illegal declaration specifiers");
                 break;
        case 25: printf("illegal functon specifiers");
                 break;
        case 26: printf("illegal or conflicting return type in function %s", s);
                 break;
        case 31: printf("undefined type for identifier %s", s);
                 break;
        case 32: printf("incomplete forward reference for identifier %s", s);
                 break;
        default: printf("unknown");
                 break;
    }
    if(strlen(yytext) == 0)
        printf(" at end\n");
    else
        printf(" near %s\n", yytext);
}
