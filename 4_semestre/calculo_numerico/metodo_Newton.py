import numpy as np

def p(x):
    return -x**4 + 7.7*x**3 - 18*x**2 + 13.6*x

def q(x):
    return -x**2 + 5*x + 0.75

def F(x, y):
    """Sistema de funções F(x, y) = 0."""
    f1 = y - p(x)
    f2 = y - q(x)
    return np.array([f1, f2])

def J(x, y):
    """Matriz Jacobiana J para F(x, y)."""
    df1_dx = 4*x**3 - 23.1*x**2 + 36*x - 13.6
    df1_dy = 1.0
    df2_dx = 2*x - 5.0
    df2_dy = 1.0

    return np.array([
        [df1_dx, df1_dy],
        [df2_dx, df2_dy]
    ])

def newton_sistemas(F, J, x0, tol, max_iter=50):
    x_k = np.array(x0)

    print("| i |    x    |    y    | ||x(k)-x(k-1)||∞ |")
    print("|---|---------|---------|------------------|")

    for k in range(max_iter):
        F_k = F(x_k[0], x_k[1])
        J_k = J(x_k[0], x_k[1])

        try:
            delta_x = np.linalg.solve(J_k, -F_k)
        except np.linalg.LinAlgError:
            print("\nErro: Matriz Jacobiana Singular. O método falhou.")
            return None

        x_k_novo = x_k + delta_x
        erro = np.linalg.norm(delta_x, ord=np.inf)

        print(f"| {k+1} | {x_k_novo[0]:.6f} | {x_k_novo[1]:.6f} | {erro:.6e} |")

        if erro < tol:
            return x_k_novo

        x_k = x_k_novo

    print("\nAviso: Máximo de iterações atingido.")
    return x_k

chute_inicial = [3.2, 6.7]
tolerancia = 0.001

print("\n### Resultados do Método de Newton para Sistemas (Exercício 2.b) ###")
raiz_n = newton_sistemas(F, J, chute_inicial, tolerancia)

if raiz_n is not None:
    print("\n--- Conclusão ---")
    print(f"Posição de Impacto (x*): {raiz_n[0]:.10f}")
    print(f"Altura de Impacto (h): {raiz_n[1]:.10f}")
    print(f"Tolerância alcançada: {tolerancia}")