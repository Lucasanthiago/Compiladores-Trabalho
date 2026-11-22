/* Scanner C-Minus com entrega de tokens sob demanda para o parser. */

#include "scanner.h"

#include <stdlib.h>
#include <string.h>

static char *source;
static size_t source_length;
static size_t position;
static int line_number = 1;
static int column_number = 1;

static int is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static void advance_one(void) {
    if (source[position] == '\r') {
        position++;
        if (position < source_length && source[position] == '\n')
            position++;
        line_number++;
        column_number = 1;
    } else if (source[position] == '\n') {
        position++;
        line_number++;
        column_number = 1;
    } else {
        position++;
        column_number++;
    }
}

static void set_token(Token *token, TokenKind kind, size_t start,
                      size_t length, int line, int column,
                      const char *error_message) {
    token->kind = kind;
    token->lexeme = source + start;
    token->length = length;
    token->line = line;
    token->column = column;
    token->error_message = error_message;
}

static TokenKind keyword_or_identifier(const char *text, size_t length) {
    static const struct {
        const char *text;
        TokenKind kind;
    } keywords[] = {
        {"else", TOKEN_ELSE}, {"if", TOKEN_IF}, {"int", TOKEN_INT},
        {"return", TOKEN_RETURN}, {"void", TOKEN_VOID},
        {"while", TOKEN_WHILE},
    };
    size_t i;

    for (i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        size_t keyword_length = strlen(keywords[i].text);
        if (length == keyword_length &&
            memcmp(text, keywords[i].text, length) == 0)
            return keywords[i].kind;
    }
    return TOKEN_ID;
}

int lexer_init(FILE *input) {
    size_t capacity = 4096;
    int ch;

    lexer_destroy();
    source = malloc(capacity);
    if (!source)
        return 0;
    while ((ch = fgetc(input)) != EOF) {
        if (source_length == capacity) {
            size_t new_capacity = capacity * 2;
            char *new_source = realloc(source, new_capacity);
            if (!new_source) {
                lexer_destroy();
                return 0;
            }
            source = new_source;
            capacity = new_capacity;
        }
        source[source_length++] = (char)ch;
    }
    if (ferror(input)) {
        lexer_destroy();
        return 0;
    }
    position = 0;
    line_number = 1;
    column_number = 1;
    return 1;
}

TokenKind lexer_next_token(Token *token) {
    size_t start;
    int start_line;
    int start_column;
    char current;

    if (!token || !source)
        return TOKEN_EOF;
    for (;;) {
        while (position < source_length &&
               (source[position] == ' ' || source[position] == '\t' ||
                source[position] == '\r' || source[position] == '\n'))
            advance_one();
        if (position + 1 < source_length && source[position] == '/' &&
            source[position + 1] == '*') {
            start = position;
            start_line = line_number;
            start_column = column_number;
            advance_one();
            advance_one();
            while (position + 1 < source_length &&
                   !(source[position] == '*' && source[position + 1] == '/'))
                advance_one();
            if (position + 1 >= source_length || source[position] != '*' ||
                source[position + 1] != '/') {
                set_token(token, TOKEN_ERROR, start, 2, start_line,
                          start_column, "comentario aberto e nao fechado");
                position = source_length;
                return TOKEN_ERROR;
            }
            advance_one();
            advance_one();
            continue;
        }
        break;
    }

    start = position;
    start_line = line_number;
    start_column = column_number;
    if (position == source_length) {
        set_token(token, TOKEN_EOF, position, 0, line_number, column_number,
                  NULL);
        return TOKEN_EOF;
    }
    current = source[position];
    if (is_letter(current)) {
        do {
            advance_one();
        } while (position < source_length && is_letter(source[position]));
        set_token(token, keyword_or_identifier(source + start,
                                                position - start),
                  start, position - start, start_line, start_column, NULL);
        return token->kind;
    }
    if (is_digit(current)) {
        do {
            advance_one();
        } while (position < source_length && is_digit(source[position]));
        if (position < source_length && is_letter(source[position])) {
            do {
                advance_one();
            } while (position < source_length && is_letter(source[position]));
            set_token(token, TOKEN_ERROR, start, position - start, start_line,
                      start_column, "numero mal formado");
            return TOKEN_ERROR;
        }
        set_token(token, TOKEN_NUM, start, position - start, start_line,
                  start_column, NULL);
        return TOKEN_NUM;
    }

    if (position + 1 < source_length) {
        TokenKind pair = TOKEN_ERROR;
        if (current == '<' && source[position + 1] == '=') pair = TOKEN_LE;
        if (current == '>' && source[position + 1] == '=') pair = TOKEN_GE;
        if (current == '=' && source[position + 1] == '=') pair = TOKEN_EQ;
        if (current == '!' && source[position + 1] == '=') pair = TOKEN_NE;
        if (pair != TOKEN_ERROR) {
            advance_one();
            advance_one();
            set_token(token, pair, start, 2, start_line, start_column, NULL);
            return pair;
        }
    }

    advance_one();
    switch (current) {
    case '+': set_token(token, TOKEN_PLUS, start, 1, start_line, start_column, NULL); return TOKEN_PLUS;
    case '-': set_token(token, TOKEN_MINUS, start, 1, start_line, start_column, NULL); return TOKEN_MINUS;
    case '*': set_token(token, TOKEN_TIMES, start, 1, start_line, start_column, NULL); return TOKEN_TIMES;
    case '/': set_token(token, TOKEN_OVER, start, 1, start_line, start_column, NULL); return TOKEN_OVER;
    case '<': set_token(token, TOKEN_LT, start, 1, start_line, start_column, NULL); return TOKEN_LT;
    case '>': set_token(token, TOKEN_GT, start, 1, start_line, start_column, NULL); return TOKEN_GT;
    case '=': set_token(token, TOKEN_ASSIGN, start, 1, start_line, start_column, NULL); return TOKEN_ASSIGN;
    case ';': set_token(token, TOKEN_SEMI, start, 1, start_line, start_column, NULL); return TOKEN_SEMI;
    case ',': set_token(token, TOKEN_COMMA, start, 1, start_line, start_column, NULL); return TOKEN_COMMA;
    case '(': set_token(token, TOKEN_LPAREN, start, 1, start_line, start_column, NULL); return TOKEN_LPAREN;
    case ')': set_token(token, TOKEN_RPAREN, start, 1, start_line, start_column, NULL); return TOKEN_RPAREN;
    case '[': set_token(token, TOKEN_LBRACKET, start, 1, start_line, start_column, NULL); return TOKEN_LBRACKET;
    case ']': set_token(token, TOKEN_RBRACKET, start, 1, start_line, start_column, NULL); return TOKEN_RBRACKET;
    case '{': set_token(token, TOKEN_LBRACE, start, 1, start_line, start_column, NULL); return TOKEN_LBRACE;
    case '}': set_token(token, TOKEN_RBRACE, start, 1, start_line, start_column, NULL); return TOKEN_RBRACE;
    default:
        set_token(token, TOKEN_ERROR, start, 1, start_line, start_column,
                  "caractere invalido");
        return TOKEN_ERROR;
    }
}

const char *token_kind_name(TokenKind kind) {
    switch (kind) {
    case TOKEN_EOF: return "EOF";
    case TOKEN_ERROR: return "ERRO_LEXICO";
    case TOKEN_ELSE: return "ELSE";
    case TOKEN_IF: return "IF";
    case TOKEN_INT: return "INT";
    case TOKEN_RETURN: return "RETURN";
    case TOKEN_VOID: return "VOID";
    case TOKEN_WHILE: return "WHILE";
    case TOKEN_LE: return "LE";
    case TOKEN_GE: return "GE";
    case TOKEN_EQ: return "EQ";
    case TOKEN_NE: return "NE";
    case TOKEN_PLUS: return "PLUS";
    case TOKEN_MINUS: return "MINUS";
    case TOKEN_TIMES: return "TIMES";
    case TOKEN_OVER: return "OVER";
    case TOKEN_LT: return "LT";
    case TOKEN_GT: return "GT";
    case TOKEN_ASSIGN: return "ASSIGN";
    case TOKEN_SEMI: return "SEMI";
    case TOKEN_COMMA: return "COMMA";
    case TOKEN_LPAREN: return "LPAREN";
    case TOKEN_RPAREN: return "RPAREN";
    case TOKEN_LBRACKET: return "LBRACKET";
    case TOKEN_RBRACKET: return "RBRACKET";
    case TOKEN_LBRACE: return "LBRACE";
    case TOKEN_RBRACE: return "RBRACE";
    case TOKEN_ID: return "ID";
    case TOKEN_NUM: return "NUM";
    }
    return "TOKEN_DESCONHECIDO";
}

void lexer_rewind(void) {
    position = 0;
    line_number = 1;
    column_number = 1;
}

void lexer_destroy(void) {
    free(source);
    source = NULL;
    source_length = 0;
    position = 0;
    line_number = 1;
    column_number = 1;
}

#ifndef SCANNER_NO_MAIN
int main(int argc, char **argv) {
    FILE *input = stdin;
    Token token;
    int errors = 0;
    int close_input = 0;

    if (argc > 1) {
        input = fopen(argv[1], "rb");
        if (!input) {
            perror(argv[1]);
            return 1;
        }
        close_input = 1;
    }
    if (!lexer_init(input)) {
        fprintf(stderr, "falha ao ler a entrada\n");
        if (close_input) fclose(input);
        return 1;
    }
    if (close_input) fclose(input);
    printf("%-5s %-5s %-10s %s\n", "LIN", "COL", "TOKEN", "LEXEMA");
    for (;;) {
        TokenKind kind = lexer_next_token(&token);
        if (kind == TOKEN_EOF) break;
        if (kind == TOKEN_ERROR) {
            fprintf(stderr, "ERRO LEXICO linha %d coluna %d: %s \"%.*s\"\n",
                    token.line, token.column, token.error_message,
                    (int)token.length, token.lexeme);
            errors++;
            continue;
        }
        printf("%-5d %-5d %-10s %.*s\n", token.line, token.column,
               token_kind_name(kind), (int)token.length, token.lexeme);
    }
    lexer_destroy();
    return errors ? 1 : 0;
}
#endif