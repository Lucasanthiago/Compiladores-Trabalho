/* CMinus.flex -- analisador lexico do C-Minus (BNF completa, 29 regras)
 *
 * Gerar:     jflex CMinus.flex
 * Compilar:  javac CMinusScanner.java
 * Usar:      java CMinusScanner arquivo.cm
 */

import java.io.*;

%%

%public
%class CMinusScanner
%unicode
%line
%column
%type Void
%xstate COMENTARIO

%{
  int erros = 0;
  private int comLinha = 0, comColuna = 0;   /* onde o comentario aberto comecou */

  private void emite(String token) {
      System.out.printf("%-5d %-5d %-10s %s%n",
                        yyline + 1, yycolumn + 1, token, yytext());
  }

  private void erro(String msg) {
      System.out.printf("ERRO LEXICO linha %d coluna %d: %s \"%s\"%n",
                        yyline + 1, yycolumn + 1, msg, yytext());
      erros++;
  }

  public static void main(String[] args) throws IOException {
      Reader entrada;
      String nome;
      if (args.length > 0) {
          entrada = new FileReader(args[0]);
          nome = args[0];
      } else {
          entrada = new InputStreamReader(System.in);
          nome = "entrada padrao";
      }

      System.out.println("== scanner gerado com JFlex -- " + nome + " ==");
      System.out.printf("%-5s %-5s %-10s %s%n", "LIN", "COL", "TOKEN", "LEXEMA");
      System.out.println("--------------------------------------------");

      CMinusScanner s = new CMinusScanner(entrada);
      s.yylex();

      System.out.println("--------------------------------------------");
      System.out.println("erros lexicos: " + s.erros);
      System.exit(s.erros == 0 ? 0 : 1);
  }
%}

letra    = [a-zA-Z]
digito   = [0-9]
ID       = {letra}+
NUM      = {digito}+
espaco   = [ \t\r\n]+

%%

<YYINITIAL> {

  /* comentarios nao geram token */
  "/*"          { comLinha = yyline + 1; comColuna = yycolumn + 1;
                  yybegin(COMENTARIO); }

  /* palavras reservadas antes de ID: em caso de empate vence a primeira regra */
  "else"        { emite("ELSE");   }
  "if"          { emite("IF");     }
  "int"         { emite("INT");    }
  "return"      { emite("RETURN"); }
  "void"        { emite("VOID");   }
  "while"       { emite("WHILE");  }

  /* operadores de dois caracteres antes dos de um */
  "<="          { emite("LE"); }
  ">="          { emite("GE"); }
  "=="          { emite("EQ"); }
  "!="          { emite("NE"); }

  "+"           { emite("PLUS");     }
  "-"           { emite("MINUS");    }
  "*"           { emite("TIMES");    }
  "/"           { emite("OVER");     }
  "<"           { emite("LT");       }
  ">"           { emite("GT");       }
  "="           { emite("ASSIGN");   }
  ";"           { emite("SEMI");     }
  ","           { emite("COMMA");    }
  "("           { emite("LPAREN");   }
  ")"           { emite("RPAREN");   }
  "["           { emite("LBRACKET"); }
  "]"           { emite("RBRACKET"); }
  "{"           { emite("LBRACE");   }
  "}"           { emite("RBRACE");   }

  /* numero colado em letra: erro lexico por regra explicita */
  {NUM}{letra}+ { erro("numero mal formado"); }

  {ID}          { emite("ID");  }
  {NUM}         { emite("NUM"); }

  {espaco}      { /* descartado */ }

  [^]           { erro("caractere invalido"); }
}

<COMENTARIO> {
  "*/"          { yybegin(YYINITIAL); }
  [^]           { /* consome o corpo do comentario */ }
}

<<EOF>> {
  if (yystate() == COMENTARIO) {
      System.out.printf("ERRO LEXICO linha %d coluna %d: "
                        + "comentario aberto e nao fechado%n", comLinha, comColuna);
      erros++;
  }
  return null;
}
