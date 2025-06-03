#ifndef SYNTAX_H

#define SYNTAX_H

#include <stdbool.h>

#include "type.h"

extern A_TYPE *int_type, *char_type, *void_type, *float_type, *string_type;
extern A_NODE *root;
extern A_ID *current_id;
extern int line_no;
extern int current_level;
extern int syntax_err;

A_NODE *makeNode(NODE_NAME name, void *a, void *b, void *c);
A_NODE *makeNodeList(NODE_NAME name, A_NODE *a, A_NODE *b);
A_ID *makeIdentifier(char *s);
A_ID *makeDummyIdentifier();
A_TYPE *makeType(T_KIND k);
A_SPECIFIER *makeSpecifier(A_TYPE *t, S_KIND s);
A_ID *searchIdentifier(char *s, A_ID *id);
A_ID *searchIdentifierAtCurrentLevel(char *s, A_ID *id);
void checkForwardReference();
void setDefaultSpecifier(A_SPECIFIER *p);
A_SPECIFIER *updateSpecifier(A_SPECIFIER *p, A_TYPE *t, S_KIND s);
A_ID *linkDeclaratorList(A_ID *id1, A_ID *id2);
A_ID *getIdentifierDeclared(char *s);
A_TYPE *getTypeOfStructOrEnumRefIdentifier(T_KIND k, char *s, ID_KIND kk);
A_NODE *setNodeInteger(A_NODE* node, int i);
A_NODE *setNodeFloat(A_NODE* node, float f);
A_NODE *setNodeChar(A_NODE* node, char c);
A_NODE *setNodeString(A_NODE* node, char *s);
A_ID *setDeclaratorInit(A_ID *id, A_NODE *node);
A_ID *setDeclaratorKind(A_ID *id, ID_KIND k);
A_ID *setDeclaratorType(A_ID *id, A_TYPE *t);
A_ID *setDeclaratorElementType(A_ID *id, A_TYPE *t);
A_ID *setDeclaratorTypeAndKind(A_ID *id, A_TYPE *t, ID_KIND k);
A_ID *setFunctionDeclaratorSpecifier(A_ID *id, A_SPECIFIER *p);
A_ID *setFunctionDeclaratorBody(A_ID *id, A_NODE *n);
A_ID *setDeclaratorListSpecifier(A_ID *id, A_SPECIFIER *p);
A_ID *setParameterDeclaratorSpecifier(A_ID *id, A_SPECIFIER *p);
A_ID *setStructDeclaratorListSpecifier(A_ID *id, A_TYPE *t);
A_TYPE *setTypeNameSpecifier(A_TYPE *t, A_SPECIFIER *p);
A_TYPE *setTypeElementType(A_TYPE *t, A_TYPE *s);
A_TYPE *setTypeField(A_TYPE *t, A_ID *n);
A_TYPE *setTypeExpr(A_TYPE *t, A_NODE *n);
A_TYPE *setTypeStructOrEnumIdentifier(T_KIND k, char *s, ID_KIND kk);
A_TYPE *setTypeAndKindOfDeclarator(A_TYPE *t, ID_KIND k, A_ID *id);
bool isNotSameFormalParameters(A_ID *a, A_ID *b);
bool isNotSameType(A_TYPE *t1, A_TYPE *t2);
bool isPointerOrArrayType(A_TYPE *t);
void initialize();
void syntax_error();

#endif