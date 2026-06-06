import numpy as np
import matplotlib.pyplot as plt

def f_runge(t):
    return 1.0 / (1.0 + 25.0 * t**2)

def lagrange_interp(t_nodes, f_nodes, t_eval):
    """
    Calcula o Polinômio de Interpolação de Lagrange no ponto t_eval.
    t_nodes: nós de interpolação (array de pontos x).
    f_nodes: valores da função nos nós (array de valores f(x)).
    t_eval: ponto ou array de pontos onde o polinômio será avaliado.
    """
    n = len(t_nodes)

    if not isinstance(t_eval, np.ndarray):
        t_eval = np.array([t_eval])

    P_t = np.zeros_like(t_eval, dtype=float)

    for i in range(n):
        L_i_t = np.ones_like(t_eval, dtype=float)
        for j in range(n):
            if i != j:
                L_i_t *= (t_eval - t_nodes[j]) / (t_nodes[i] - t_nodes[j])

        P_t += f_nodes[i] * L_i_t

    return P_t

def newton_diff_div(t_nodes, f_nodes):
    """
    Calcula a tabela de diferenças divididas para o Polinômio de Newton.
    Retorna o vetor de coeficientes (a primeira linha da matriz de diferenças divididas).
    """
    n = len(t_nodes)
    F = np.zeros((n, n))
    F[:, 0] = f_nodes

    for j in range(1, n):
        for i in range(n - j):
            F[i, j] = (F[i+1, j-1] - F[i, j-1]) / (t_nodes[i+j] - t_nodes[i])

    return F[0, :]

def newton_interp(t_nodes, coeffs, t_eval):
    """
    Avalia o Polinômio de Interpolação de Newton no ponto t_eval.
    t_nodes: nós de interpolação.
    coeffs: coeficientes (diferenças divididas) calculados por newton_diff_div.
    t_eval: ponto ou array de pontos onde o polinômio será avaliado.
    """
    n = len(t_nodes)

    if not isinstance(t_eval, np.ndarray):
        t_eval = np.array([t_eval])

    P_t = np.zeros_like(t_eval, dtype=float)

    for k in range(len(t_eval)):
        P_t[k] = coeffs[0]
        temp_prod = 1.0

        for i in range(1, n):
            temp_prod *= (t_eval[k] - t_nodes[i-1])
            P_t[k] += coeffs[i] * temp_prod

    return P_t
