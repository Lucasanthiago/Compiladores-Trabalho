#ifndef CMINUS_AST_H
#define CMINUS_AST_H

#include <stddef.h>
#include <stdio.h>

typedef struct AstNode AstNode;

struct AstNode {
    const char *name;
    char *lexeme;
    int line;
    int column;
    size_t child_count;
    AstNode **children;
};

AstNode *ast_new(const char *name, size_t child_count, ...);
AstNode *ast_token(const char *name, const char *lexeme, size_t length,
                   int line, int column);
AstNode *ast_empty(void);
void ast_print(FILE *output, const AstNode *node, size_t depth);
void ast_free(AstNode *node);

#endif
