/* Sub.flex -- analisador lexico do SUBCONJUNTO (alfabeto S = {0-9, +, -})
 *
 * Mesma linguagem do scanner manual, para a comparacao da secao 9.
 *
 * Gerar:     jflex Sub.flex
 * Compilar:  javac SubScanner.java
 * Usar:      java SubScanner arquivo.txt
 */

import java.io.*;

%%

%public
%class SubScanner
%unicode
%line
%column
%type Void

%{
  int erros = 0;

  private void emite(String token) {
      System.out.printf("%-5d %-5d %-8s %s%n",
                        yyline + 1, yycolumn + 1, token, yytext());
  }

  public static void main(String[] args) throws IOException {
      Reader entrada;
      String nome;
      if (args.length > 0) { entrada = new FileReader(args[0]); nome = args[0]; }
      else { entrada = new InputStreamReader(System.in); nome = "entrada padrao"; }

      System.out.println("== scanner gerado com JFlex (subconjunto) -- " + nome + " ==");
      System.out.printf("%-5s %-5s %-8s %s%n", "LIN", "COL", "TOKEN", "LEXEMA");
      System.out.println("-----------------------------------------");

      SubScanner s = new SubScanner(entrada);
      s.yylex();

      System.out.println("-----------------------------------------");
      System.out.println("erros lexicos: " + s.erros);
      System.exit(s.erros == 0 ? 0 : 1);
  }
%}

digito = [0-9]
NUM    = {digito}+
addop  = [+\-]
espaco = [ \t\r\n]+

%%

{NUM}     { emite("NUM");   }
{addop}   { emite("addop"); }
{espaco}  { /* descartado */ }
[^]       { System.out.printf("ERRO LEXICO linha %d coluna %d: "
                              + "caractere fora do alfabeto \"%s\"%n",
                              yyline + 1, yycolumn + 1, yytext());
            erros++; }

<<EOF>>   { return null; }
