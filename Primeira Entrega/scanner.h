#ifndef CMINUS_SCANNER_H
#define CMINUS_SCANNER_H

#include <stddef.h>
#include <stdio.h>

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_ERROR = 256,
    TOKEN_ELSE,
    TOKEN_IF,
    TOKEN_INT,
    TOKEN_RETURN,
    TOKEN_VOID,
    TOKEN_WHILE,
    TOKEN_LE,
    TOKEN_GE,
    TOKEN_EQ,
    TOKEN_NE,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_TIMES,
    TOKEN_OVER,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_ASSIGN,
    TOKEN_SEMI,
    TOKEN_COMMA,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_ID,
    TOKEN_NUM
} TokenKind;

typedef struct {
    TokenKind kind;
    const char *lexeme;
    size_t length;
    int line;
    int column;
    const char *error_message;
} Token;

int lexer_init(FILE *input);
void lexer_rewind(void);
TokenKind lexer_next_token(Token *token);
void lexer_destroy(void);
const char *token_kind_name(TokenKind kind);

#endif