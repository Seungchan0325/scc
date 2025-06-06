#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "type.h"
#include "syntax.h"
#include "printAST.h"

FILE *fout;

int yyparse();

int main()
{
    if((fout = fopen("a.txt", "w")) == NULL) {
        printf("can not open output file: a.txt\n");
        return 1;
    }
    initialize();
    yyparse();
    if(syntax_err) return 1;
    print_ast(root);
    // printProgram(root, 0);
    return 0;
}