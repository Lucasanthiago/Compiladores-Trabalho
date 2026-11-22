CC ?= gcc
BISON ?= bison
CFLAGS ?= -Wall

all: scanner jflex parser

scanner: scanner.c
	gcc -Wall -o scanner scanner.c

jflex:
	jflex Sub.flex
	jflex CMinus.flex
	javac SubScanner.java CMinusScanner.java

parser: parser.tab.c scanner_adaptado.c scanner.h ast.c ast.h
	$(CC) $(CFLAGS) -DSCANNER_NO_MAIN -o cminus_parser parser.tab.c scanner_adaptado.c ast.c

parser.tab.c: parser.y scanner.h
	$(BISON) -Wall -Wcounterexamples -d -o $@ parser.y

test: all
	./scanner valido.txt            > saidas/manual_valido.txt       2>&1 || true
	./scanner invalido.txt          > saidas/manual_invalido.txt     2>&1 || true
	java SubScanner valido.txt      > saidas/jflex_sub_valido.txt    2>&1 || true
	java SubScanner invalido.txt    > saidas/jflex_sub_invalido.txt  2>&1 || true
	java CMinusScanner valido.cm    > saidas/jflex_valido.txt        2>&1 || true
	java CMinusScanner invalido.cm  > saidas/jflex_invalido.txt      2>&1 || true

test-parser: parser
	@./cminus_parser testes/parser_valido_funcoes.cm > /tmp/parser_valido_funcoes.out
	@diff -u saidas/parser_valido_funcoes.expected /tmp/parser_valido_funcoes.out
	@./cminus_parser testes/parser_valido_array_while.cm > /tmp/parser_valido_array_while.out
	@diff -u saidas/parser_valido_array_while.expected /tmp/parser_valido_array_while.out
	@if ./cminus_parser testes/parser_invalido_ponto_virgula.cm > /tmp/parser_invalido_ponto_virgula.out 2>&1; then \
		echo "ERRO: o parser aceitou o programa que deveria ser invalido"; exit 1; \
	fi
	@diff -u saidas/parser_invalido_ponto_virgula.expected /tmp/parser_invalido_ponto_virgula.out
	@echo "Testes do parser passaram (2 validos, 1 invalido)."

test-expressions:
	@python3 AnlMathExp.py --file testes/expressao_valida_precedencia.expr > /tmp/expressao_valida_precedencia.out
	@diff -u saidas/expressao_valida_precedencia.expected /tmp/expressao_valida_precedencia.out
	@python3 AnlMathExp.py --file testes/expressao_valida_decimal_potencia.expr > /tmp/expressao_valida_decimal_potencia.out
	@diff -u saidas/expressao_valida_decimal_potencia.expected /tmp/expressao_valida_decimal_potencia.out
	@if python3 AnlMathExp.py --file testes/expressao_invalida_operador.expr > /tmp/expressao_invalida_operador.out 2>&1; then \
		echo "ERRO: o parser LL(1) aceitou a expressao invalida"; exit 1; \
	fi
	@diff -u saidas/expressao_invalida_operador.expected /tmp/expressao_invalida_operador.out
	@echo "Testes das expressoes passaram (2 validas, 1 invalida)."

test-original: parser
	@./cminus_parser valido.cm > /tmp/cminus_original_valido.out
	@diff -u saidas/cminus_original_valido.expected /tmp/cminus_original_valido.out
	@if ./cminus_parser invalido.cm > /tmp/cminus_original_invalido.out 2>&1; then \
		echo "ERRO: o parser aceitou o arquivo invalido.cm"; exit 1; \
	fi
	@diff -u saidas/cminus_original_invalido.expected /tmp/cminus_original_invalido.out
	@echo "Testes originais passaram (valido.cm valido, invalido.cm invalido)."

test-all: test-expressions test-parser test-original
	@echo "Validacao completa: todos os testes passaram."

compara: test
	grep "^[0-9]" saidas/manual_valido.txt    > /tmp/a.txt
	grep "^[0-9]" saidas/jflex_sub_valido.txt > /tmp/b.txt
	diff /tmp/a.txt /tmp/b.txt && echo "SAIDAS IDENTICAS token a token"

clean:
	rm -f scanner cminus_parser parser.tab.c parser.tab.h *.class SubScanner.java CMinusScanner.java

.PHONY: all jflex parser test test-parser test-expressions test-original test-all compara clean
