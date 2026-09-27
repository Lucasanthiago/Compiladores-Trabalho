/* scanner.c -- analisador lexico do subconjunto (regras 18, 20, 22, 23, 24, 26
 *              na forma restrita, alfabeto S = {0-9, +, -})
 *
 * Implementado diretamente a partir da tabela delta do DFA minimo da secao 8.5
 * do relatorio. Tres estados uteis mais o estado de erro:
 *
 *        |  D    addop        token
 *   -----+---------------------------
 *   ->q0 |  q1   q2           --
 *     q1 |  q1   erro         NUM
 *     q2 |  erro erro         addop
 *
 * Compilar:  gcc -Wall -o scanner scanner.c
 * Usar:      ./scanner arquivo.txt
 */

#include <stdio.h>
#include <stdlib.h>

#define ERRO   -1
#define NCLASSE 2      /* colunas: 0 = D (digito), 1 = addop (+ ou -) */
#define NESTADO 3
#define INICIAL 0

/* tabela de transicao do DFA minimo; -1 = estado de erro */
static const int delta[NESTADO][NCLASSE] = {
    /*      D     addop */
    /* q0 */ { 1,    2    },
    /* q1 */ { 1,    ERRO },
    /* q2 */ { ERRO, ERRO },
};

/* token reconhecido em cada estado; NULL = estado nao final */
static const char *const token_de[NESTADO] = {
    NULL,      /* q0 */
    "NUM",     /* q1 */
    "addop",   /* q2 */
};

/* caractere -> coluna da tabela; -1 se estiver fora do alfabeto */
static int classe(int c) {
    if (c >= '0' && c <= '9') return 0;
    if (c == '+' || c == '-') return 1;
    return -1;
}

static char *buf;
static long  tam, pos = 0;
static int   linha = 1, coluna = 1, erros = 0;

static void avanca(void) {
    if (buf[pos] == '\n') { linha++; coluna = 1; }
    else                  { coluna++; }
    pos++;
}

/* espacos em branco nao geram token */
static int pula_brancos(void) {
    while (pos < tam && (buf[pos] == ' '  || buf[pos] == '\t' ||
                         buf[pos] == '\n' || buf[pos] == '\r'))
        avanca();
    return pos < tam;
}

int main(int argc, char **argv) {
    FILE *f = stdin;
    long cap = 4096;
    int ch;

    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc > 1) {
        f = fopen(argv[1], "rb");
        if (!f) { perror(argv[1]); return 1; }
    }
    /* le tudo: o recuo do maior casamento precisa voltar atras na entrada */
    buf = malloc(cap);
    while ((ch = fgetc(f)) != EOF) {
        if (tam + 1 >= cap) { cap *= 2; buf = realloc(buf, cap); }
        buf[tam++] = (char)ch;
    }
    buf[tam] = '\0';
    if (f != stdin) fclose(f);

    printf("== scanner do subconjunto (DFA minimo, 3 estados) -- %s ==\n",
           argc > 1 ? argv[1] : "entrada padrao");
    printf("%-5s %-5s %-8s %s\n", "LIN", "COL", "TOKEN", "LEXEMA");
    printf("-----------------------------------------\n");

    while (pula_brancos()) {
        int  estado = INICIAL;
        long ini = pos;
        int  lin_ini = linha, col_ini = coluna;

        /* maior casamento possivel: guarda o ultimo estado final visitado */
        const char *ult_token = NULL;
        long ult_pos = -1;
        int  ult_linha = linha, ult_coluna = coluna;

        long p = pos;
        int  l = linha, c = coluna;
        while (p < tam) {
            int k = classe((unsigned char)buf[p]);
            if (k < 0) break;                 /* fora do alfabeto */
            int prox = delta[estado][k];
            if (prox == ERRO) break;          /* sem transicao: travou */
            estado = prox;
            if (buf[p] == '\n') { l++; c = 1; } else { c++; }
            p++;
            if (token_de[estado]) {           /* passou por um estado final */
                ult_token  = token_de[estado];
                ult_pos    = p;
                ult_linha  = l;
                ult_coluna = c;
            }
        }

        if (ult_token) {                      /* recua ate o ultimo final */
            printf("%-5d %-5d %-8s %.*s\n", lin_ini, col_ini, ult_token,
                   (int)(ult_pos - ini), buf + ini);
            pos = ult_pos; linha = ult_linha; coluna = ult_coluna;
        } else {
            fprintf(stderr, "ERRO LEXICO linha %d coluna %d: "
                    "caractere fora do alfabeto \"%c\"\n", linha, coluna, buf[pos]);
            erros++;
            avanca();                         /* segue para nao parar no 1o erro */
        }
    }

    printf("-----------------------------------------\n");
    printf("erros lexicos: %d\n", erros);
    free(buf);
    return erros ? 1 : 0;
}
