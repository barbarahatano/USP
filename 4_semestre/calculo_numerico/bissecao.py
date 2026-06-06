import numpy as np
def p(x):
    """Perfil da elevação."""
    return -x**4 + 7.7*x**3 - 18*x**2 + 13.6*x

def q(x):
    """Curva do projétil."""
    return -x**2 + 5*x + 0.75

def f(x):
    """Função cuja raiz é procurada: f(x) = p(x) - q(x)."""
    return p(x) - q(x)
def bissecao(f, a, b, tol, max_iter):
    if f(a) * f(b) >= 0:
        print("Erro: f(a) e f(b) devem ter sinais opostos. Tente outro intervalo.")
        return None, None

    iteracoes = 0
    print("| i |   a    |   b    |   xm   | f(xm)  | |b-a| ")
    print("|---|--------|--------|--------|--------|-------|")

    while (b - a) / 2 > tol and iteracoes < max_iter:
        xm = (a + b) / 2  
        f_a = f(a)
        f_xm = f(xm)
        print(f"| {iteracoes+1} | {a:.4f} | {b:.4f} | {xm:.4f} | {f_xm:.4f} | {b-a:.4f} |")

        if f_xm == 0:
            a = b = xm
            break
        elif f_a * f_xm < 0:
            b = xm
        else:
            a = xm

        iteracoes += 1

    x_aprox = (a + b) / 2
    return x_aprox, q(x_aprox)

a_bissecao = 3.1
b_bissecao = 3.2
tolerancia = 0.001
max_i_bissecao = 5

print("### Resultados do Método da Bisseção (Exercício 2.a) ###")
raiz_b, altura_b = bissecao(f, a_bissecao, b_bissecao, tolerancia, max_i_bissecao)

if raiz_b is not None:
    print("\n--- Conclusão ---")
    print(f"Posição de Impacto (x*): {raiz_b:.10f}")
    print(f"Altura de Impacto (h): {altura_b:.10f}")
    print(f"Iterações realizadas: {max_i_bissecao} (Limite atingido)")