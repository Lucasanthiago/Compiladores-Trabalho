#include "ast.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static char *copy_text(const char *text, size_t length) {
    char *copy = malloc(length + 1);
    if (!copy)
        abort();
    memcpy(copy, text, length);
    copy[length] = '\0';
    return copy;
}

AstNode *ast_new(const char *name, size_t child_count, ...) {
    AstNode *node = calloc(1, sizeof(*node));
    size_t i;
    va_list children;

    if (!node)
        abort();
    node->name = name;
    node->child_count = child_count;
    if (child_count) {
        node->children = calloc(child_count, sizeof(*node->children));
        if (!node->children)
            abort();
    }

    va_start(children, child_count);
    for (i = 0; i < child_count; i++)
        node->children[i] = va_arg(children, AstNode *);
    va_end(children);
    return node;
}

AstNode *ast_token(const char *name, const char *lexeme, size_t length,
                   int line, int column) {
    AstNode *node = calloc(1, sizeof(*node));
    if (!node)
        abort();
    node->name = name;
    node->lexeme = copy_text(lexeme, length);
    node->line = line;
    node->column = column;
    return node;
}

AstNode *ast_empty(void) {
    return ast_new("epsilon", 0);
}

void ast_print(FILE *output, const AstNode *node, size_t depth) {
    size_t i;
    if (!node)
        return;

    fprintf(output, "%*s%s", (int)(depth * 2), "", node->name);
    if (node->lexeme)
        fprintf(output, ": %s (linha %d, coluna %d)", node->lexeme,
                node->line, node->column);
    fputc('\n', output);

    for (i = 0; i < node->child_count; i++)
        ast_print(output, node->children[i], depth + 1);
}

void ast_free(AstNode *node) {
    size_t i;
    if (!node)
        return;
    for (i = 0; i < node->child_count; i++)
        ast_free(node->children[i]);
    free(node->children);
    free(node->lexeme);
    free(node);
}
