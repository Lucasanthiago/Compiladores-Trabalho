import sys

DIGITOS = "0123456789"
NAO_TERMINAIS = {"E", "E'", "T", "T'", "F", "F'", "P"}

class ErroLexico(Exception): pass
class ErroSintatico(Exception): pass

class Token:
    def __init__(self, tipo, lexema, col):
        self.tipo, self.lexema, self.col = tipo, lexema, col

# ---------- Léxico (DFA: q0..q3) ----------
def lexer(texto):
    toks, i, n = [], 0, len(texto)
    while i < n:
        c = texto[i]
        if c in " \t\r\n":
            i += 1
        elif c in "+-*/^()":
            toks.append(Token(c, c, i + 1)); i += 1
        elif c in DIGITOS:
            ini, estado = i, 1
            i += 1
            while i < n:
                c = texto[i]
                if estado == 1 and c in DIGITOS: i += 1
                elif estado == 1 and c == ".": estado = 2; i += 1
                elif estado in (2, 3) and c in DIGITOS: estado = 3; i += 1
                else: break
            if estado == 2:
                raise ErroLexico(f"coluna {ini+1}: decimal incompleto '{texto[ini:i]}'")
            toks.append(Token("num", texto[ini:i], ini + 1))
        else:
            raise ErroLexico(f"coluna {i+1}: caractere fora do alfabeto '{c}'")
    toks.append(Token("$", "$", n + 1))
    return toks

# ---------- Tabela LL(1) ----------
TABELA = {}
def prod(nt, terminais, rhs):
    for t in terminais:
        TABELA[(nt, t)] = rhs

prod("E",  ["(", "num"], ["T", "E'"])
prod("E'", ["+"], ["+", "T", "E'"])
prod("E'", ["-"], ["-", "T", "E'"])
prod("E'", [")", "$"], [])
prod("T",  ["(", "num"], ["F", "T'"])
prod("T'", ["*"], ["*", "F", "T'"])
prod("T'", ["/"], ["/", "F", "T'"])
prod("T'", ["+", "-", ")", "$"], [])
prod("F",  ["(", "num"], ["P", "F'"])
prod("F'", ["^"], ["^", "F"])
prod("F'", ["*", "/", "+", "-", ")", "$"], [])
prod("P",  ["("], ["(", "E", ")"])
prod("P",  ["num"], ["num"])

# ---------- Árvore ----------
class No:
    def __init__(self, simbolo):
        self.simbolo, self.lexema, self.filhos = simbolo, None, []

def imprime(no, prefixo="", ultimo=True, raiz=True):
    rotulo = f"num({no.lexema})" if no.simbolo == "num" else no.simbolo
    if raiz:
        print(rotulo); novo = ""
    else:
        print(prefixo + ("└── " if ultimo else "├── ") + rotulo)
        novo = prefixo + ("    " if ultimo else "│   ")
    for k, f in enumerate(no.filhos):
        imprime(f, novo, k == len(no.filhos) - 1, False)

# ---------- Parser ----------
def parse(toks):
    raiz = No("E")
    pilha = [No("$"), raiz]          # topo = fim da lista
    i, passo = 0, 0
    print(f"{'Passo':<6}{'Pilha':<28}{'Entrada':<28}Ação")
    while True:
        topo, tok = pilha[-1], toks[i]
        passo += 1
        s_pilha = " ".join(n.simbolo for n in reversed(pilha))
        s_ent = " ".join(t.tipo for t in toks[i:])

        if topo.simbolo in NAO_TERMINAIS:
            rhs = TABELA.get((topo.simbolo, tok.tipo))
            if rhs is None:
                esp = sorted(t for (nt, t) in TABELA if nt == topo.simbolo)
                raise ErroSintatico(
                    f"coluna {tok.col}: encontrou '{tok.tipo}', esperava {esp}")
            pilha.pop()
            if rhs == []:
                topo.filhos = [No("ε")]
                acao = f"{topo.simbolo} → ε"
            else:
                topo.filhos = [No(s) for s in rhs]
                pilha.extend(reversed(topo.filhos))
                acao = f"{topo.simbolo} → {' '.join(rhs)}"
        elif topo.simbolo == tok.tipo:
            if topo.simbolo == "$":
                print(f"{passo:<6}{s_pilha:<28}{s_ent:<28}ACEITA")
                return raiz
            topo.lexema = tok.lexema
            pilha.pop(); i += 1
            acao = f"casa '{tok.tipo}'"
        else:
            raise ErroSintatico(
                f"coluna {tok.col}: encontrou '{tok.tipo}', esperava '{topo.simbolo}'")
        print(f"{passo:<6}{s_pilha:<28}{s_ent:<28}{acao}")

if __name__ == "__main__":
    expr = " ".join(sys.argv[1:]) or input("Expressão: ")
    try:
        arvore = parse(lexer(expr))
        print("\nÁrvore sintática:")
        imprime(arvore)
    except (ErroLexico, ErroSintatico) as e:
        print(f"\nERRO {type(e).__name__}: {e}")