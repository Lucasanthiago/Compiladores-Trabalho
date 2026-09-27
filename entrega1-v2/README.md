# Entrega 1 — Analisador Léxico (C-Minus)

Duas trilhas sobre a mesma linguagem, comparadas ao final.

## Trilha manual — subconjunto

Regras 18, 20, 22, 23, 24 e 26 na forma restrita, alfabeto Σ = {0-9, +, -} e
dois tokens: `NUM` e `addop`.

`scanner.c` é escrito diretamente a partir da tabela δ do DFA mínimo de três
estados da seção 8.5 do relatório. A tabela está no topo do arquivo, em nove
linhas, e o resto é o motor: maior casamento possível, contagem de linha e
coluna, e relato de caractere fora do alfabeto sem parar no primeiro erro.

## Trilha com ferramenta — JFlex

`Sub.flex` cobre exatamente o mesmo subconjunto, para a comparação ser direta.
`CMinus.flex` cobre a BNF completa das 29 regras, como pede o enunciado.

Números que o próprio JFlex reporta:

| Especificação | NFA | DFA | Mínimo |
|---|---|---|---|
| `Sub.flex` | 14 | 8 | 5 |
| `CMinus.flex` | 106 | 59 | 54 |

## Construir e rodar

```bash
make            # compila o scanner em C e gera os dois em Java
make test       # roda as seis execuções
make compara    # confere se as duas trilhas batem token a token
```

Dependências: `gcc`, `jflex` e um JDK.

## Resultado

Sobre `valido.txt` as duas trilhas produzem as mesmas 26 linhas, token a token,
com as mesmas linhas e colunas. `make compara` verifica isso e imprime
`SAIDAS IDENTICAS token a token`.
