# Organização dos códigos e das saídas

## Parte I — Parser LL(1) de expressões matemáticas

Código principal:

- `AnlMathExp.py`

Casos de entrada:

- `testes/expressao_valida_precedencia.expr`
- `testes/expressao_valida_decimal_potencia.expr`
- `testes/expressao_invalida_operador.expr`

Saídas esperadas:

- `saidas/expressao_valida_precedencia.expected`
- `saidas/expressao_valida_decimal_potencia.expected`
- `saidas/expressao_invalida_operador.expected`

## Parte II — Bison e C-Minus

Código principal:

- `parser.y`
- `scanner_adaptado.c`
- `scanner.h`
- `ast.c`
- `ast.h`
- `parser.tab.c`
- `parser.tab.h`

Casos de entrada criados:

- `testes/parser_valido_funcoes.cm`
- `testes/parser_valido_array_while.cm`
- `testes/parser_invalido_ponto_virgula.cm`

Saídas esperadas:

- `saidas/parser_valido_funcoes.expected`
- `saidas/parser_valido_array_while.expected`
- `saidas/parser_invalido_ponto_virgula.expected`

Casos da entrega original:

- `valido.cm` e `saidas/cminus_original_valido.expected`
- `invalido.cm` e `saidas/cminus_original_invalido.expected`

# Comandos de compilação e execução

Gerar e compilar o parser Bison:

```bash
make parser
```

Executar um programa C-Minus:

```bash
./cminus_parser caminho/do/programa.cm
```

Executar uma expressão pelo parser LL(1):

```bash
python3 AnlMathExp.py --file testes/expressao_valida_precedencia.expr
```

## Execução dos testes

Executar todos os testes do projeto:

```bash
make test-all
```

Executar cada grupo separadamente:

```bash
make test-expressions
make test-parser
make test-original
```

Limpar os artefatos gerados:

```bash
make clean
```

## Resultado esperado dos grupos

- `test-expressions`: dois casos válidos e um caso inválido.
- `test-parser`: dois programas C-Minus válidos e um programa sintaticamente inválido.
- `test-original`: `valido.cm` aceito e `invalido.cm` rejeitado por erros léxicos.
- `test-all`: todos os grupos anteriores executados com sucesso.

## Confirmação dos tipos de erro

Erros léxicos são apresentados por:

```text
Erro lexico na linha X, coluna Y: descricao
```

Os erros léxicos registrados no arquivo `invalido.cm` incluem número malformado, caractere inválido e comentário não fechado.

Erros sintáticos são apresentados por:

```text
Erro sintatico na linha X, coluna Y: descricao detalhada
Codigo invalido.
```

Entradas válidas são identificadas por:

```text
Codigo sintaticamente valido.
Arvore sintatica:
```
