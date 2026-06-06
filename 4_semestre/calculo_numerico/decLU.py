import numpy as np
import time

def func1 ( A ) :
  n = A.shape[0]
  U = A.copy()
  L = np.eye(n)
  for j in range ( n - 1 ) :
    for i in range ( j + 1 , n ) :
      L [ i , j ] = U[ i , j ] / U[ j , j ]
      U[ i , j : n ] = U[ i , j : n ] - L [ i , j ] * U[ j , j : n ]
  return ( L, U )

def func2(A,p):
  n=A.shape[0]
  U=A.copy()
  L=np.eye(n)

  for j in range(n-1):
    v = min(n, j+p+1)
    for i in range(j+1, v):
      L[i,j]= U[i,j]/U[j,j]
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