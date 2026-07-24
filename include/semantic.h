#ifndef SEMANTIC_H

#define SEMANTIC_H

#include <stdbool.h>
#include "type.h"

#define LIT_MAX 100

void semantic_analysis(A_NODE *);
void sem_program(A_NODE *);
void sem_declaration_list(A_ID *, int);
int sem_declaration(A_ID *, int);
int sem_A_TYPE(A_TYPE *);
A_TYPE *sem_expression(A_NODE *);
int sem_statement(A_NODE *, int);
int sem_statement_list(A_NODE *, int);
void sem_for_expression(A_NODE *);
A_TYPE *sem_initializer(A_NODE *);
A_ID *sem_arg_expr_list(A_NODE *, A_ID *);

A_ID *getStructFieldIdentifier(A_TYPE *, char *);
A_ID *getPointerFieldIdentifier(A_TYPE *, char *);
A_NODE *convertScalarToInteger(A_NODE *);
A_NODE *convertUsualAssignmentConversion(A_TYPE *, A_NODE *);
A_NODE *convertUsualUnaryConverion(A_NODE *);
A_TYPE *convertUsualBinaryConversion(A_NODE *);
A_NODE *convertCastingConversion(A_NODE *, A_TYPE *);
bool isAllowableAssignmentConversion(A_TYPE *, A_TYPE *, A_NODE *);
bool isAllowableCastingConversion(A_TYPE *, A_TYPE *);
bool isModifiableLvalue(A_NODE *);
bool isConstantZeroExp(A_NODE *);
bool isSameParameterType(A_ID *, A_ID *);
bool isCompatibleType(A_TYPE *, A_TYPE *);
bool isCompatiblePointerType(A_TYPE *, A_TYPE *);
bool isIntType(A_TYPE *);
bool isFloatType(A_TYPE *);
bool isArithmeticType(A_TYPE *);
bool isScalarType(A_TYPE *);
bool isAnyIntegerType(A_TYPE *);
bool isIntegralType(A_TYPE *);
bool isStructOrUnionType(A_TYPE *);
bool isFunctionType(A_TYPE *);
bool isPointerType(A_TYPE *);
bool isPointerOrArrayType(A_TYPE *);
bool isArrayType(A_TYPE *);
bool isStringType(A_TYPE *);
bool isVoidType(A_TYPE *);

A_LITERAL checkTypeAndConvertLiteral(A_LITERAL, A_TYPE *);
A_LITERAL getTypeAndValueOfExpression(A_NODE *);
void semantic_warning(int, int);
void semantic_error();
int put_literal(A_LITERAL);
void set_literal_address(A_NODE *);

#endif
