import numpy as np

def metodo_francis(B, max_iter=100, tol=1e-8):
  
    B_k = B.copy()
    n = B_k.shape[0]
    V = np.eye(n)

    for k in range(1, max_iter + 1):
        Q_k, R_k = np.linalg.qr(B_k)
        B_k_next = R_k @ Q_k
        V = V @ Q_k
        diag_sum_sq = np.sum(np.diag(B_k_next)**2)
        norm_F_sq = np.sum(B_k_next**2)
        off_B = np.sqrt(norm_F_sq - diag_sum_sq)

        if off_B < tol:
            print(f"Convergência alcançada na iteração {k} (off(B) = {off_B:.2e} < {tol:.0e})")
            return B_k_next, V

        B_k = B_k_next

    print(f"Aviso: Máximo de {max_iter} iterações atingido (off(B) = {off_B:.2e}).")
    return B_k, V

B_test = np.array([[4, 1, 1], [1, 3, 2], [1, 2, 5]])
Autovalores, Autovetores = metodo_francis(B_test)
print("\nAutovalores (Diagonal de B_k):\n", np.diag(Autovalores))
print("\nAutovetores (Colunas de V):\n", Autovetores)

def svd_francis(A, tol_francis=1e-10):

    m, n = A.shape
    B = A.T @ A
    Sigma_sq_mat, V = metodo_francis(B, tol=tol_francis)
    autovalores = np.diag(Sigma_sq_mat)
    idx = np.argsort(autovalores)[::-1]
    autovalores = autovalores[idx]
    V = V[:, idx]
    S_vec = np.sqrt(np.maximum(autovalores, 0.0))
    k = min(m, n)
    U = np.zeros((m, m))

    for i in range(k):
        sigma_i = S_vec[i]
        if sigma_i > 1e-10:
            u_i = (A @ V[:, i]) / sigma_i
            U[:, i] = u_i
        else:
            vec = A @ V[:, i]
            norm_vec = np.linalg.norm(vec)
            if norm_vec > 1e-10:
                 U[:, i] = vec / norm_vec

    if m > k:
        Q_temp, _ = np.linalg.qr(np.random.rand(m, m))
        U[:, k:] = Q_temp[:, k:]

    VT = V.T
    return U, S_vec, VT