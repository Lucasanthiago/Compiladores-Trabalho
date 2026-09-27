all: scanner jflex

scanner: scanner.c
	gcc -Wall -o scanner scanner.c

jflex:
	jflex Sub.flex
	jflex CMinus.flex
	javac SubScanner.java CMinusScanner.java

test: all
	./scanner valido.txt            > saidas/manual_valido.txt       2>&1 || true
	./scanner invalido.txt          > saidas/manual_invalido.txt     2>&1 || true
	java SubScanner valido.txt      > saidas/jflex_sub_valido.txt    2>&1 || true
	java SubScanner invalido.txt    > saidas/jflex_sub_invalido.txt  2>&1 || true
	java CMinusScanner valido.cm    > saidas/jflex_valido.txt        2>&1 || true
	java CMinusScanner invalido.cm  > saidas/jflex_invalido.txt      2>&1 || true

compara: test
	grep "^[0-9]" saidas/manual_valido.txt    > /tmp/a.txt
	grep "^[0-9]" saidas/jflex_sub_valido.txt > /tmp/b.txt
	diff /tmp/a.txt /tmp/b.txt && echo "SAIDAS IDENTICAS token a token"

clean:
	rm -f scanner *.class SubScanner.java CMinusScanner.java

.PHONY: all jflex test compara clean
