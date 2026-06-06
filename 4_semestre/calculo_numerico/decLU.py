import numpy as np
import time

def func1 ( A ) :   #Eliminação de Gauss
  n = A.shape[0]
  U = A.copy() # Inicializa U como uma cópia da matriz original A
  L = np.eye(n) # Inicializa L como uma matriz identidade n x n
  for j in range ( n - 1 ) : # Loop pelas colunas (exceto a última)
    for i in range ( j + 1 , n ) : # Loop pelas linhas abaixo da diagonal principal
      # Calcula o multiplicador L[i,j] que zera o elemento U[i,j]
      L [ i , j ] = U[ i , j ] / U[ j , j ]
      # Atualiza a linha i da matriz U, aplicando a operação de eliminação
      # Subtrai L[i,j] * linha j de U da linha i de U
      U[ i , j : n ] = U[ i , j : n ] - L [ i , j ] * U[ j , j : n ]
  return ( L, U )

def func2(A,p):
  n=A.shape[0]
  U=A.copy() # Inicializa U como uma cópia da matriz original A
  L=np.eye(n) # Inicializa L como uma matriz identidade n x n

  for j in range(n-1): # Loop pelas colunas (exceto a última)
    # 'v' define o limite superior para o loop das linhas, considerando a largura de banda 'p'
    # Isso otimiza o cálculo para matrizes de banda, evitando operações desnecessárias
    v = min(n, j+p+1)
    for i in range(j+1, v): # Loop pelas linhas abaixo da diagonal, dentro da banda
      # Calcula o multiplicador L[i,j] que zera o elemento U[i,j]
      L[i,j]= U[i,j]/U[j,j]
      # Atualiza a linha i da matriz U, aplicando a operação de eliminação
      # A operação é restrita à largura de banda 'v' para otimização
      U[i,j:v]=U[i,j:v]-L[i,j]*U[j,j:v]
  return (L,U)

n = 2000
p=2
A=np.zeros((n,n))

for i in range (n):
  for j in range(max(0, i-p), min(n, i+p+1)):
    A[i,j]=np.random.normal()

start_time=time.time()
(L, U)=func1(A)
end_time=time.time()
print(end_time-start_time)

start_time=time.time()
(L_,U_)=func2(A,p)
end_time=time.time()
print(end_time-start_time)

print(np.linalg.norm(L@U-A))
print(np.linalg.norm(L_ @ U_ -A))