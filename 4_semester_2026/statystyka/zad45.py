import numpy as np
import matplotlib.pyplot as plt

n = 500
wymiary = [1, 2, 5, 10, 50, 100, 500] 
wartosci_R = [] 

for d in wymiary:
    x = np.random.uniform(0, 1, (n, d))

    odleglosci = np.zeros((n, n))
    for i in range(n):
        for j in range(n):
            odleglosci[i][j] = np.linalg.norm(x[i] - x[j])
    
    suma_R = 0 
    
    for i in range(n):
        dmax = np.max(odleglosci[i]) 
        odleglosci[i][i] = np.inf 
        dmin = np.min(odleglosci[i]) 
        R_i = (dmax - dmin) / dmin 
        suma_R += R_i 

    R = suma_R / n
    wartosci_R.append(R) 


print("Wyliczone wartości R:", wartosci_R)


plt.plot(wymiary, wartosci_R, marker='o')
plt.xlabel('Liczba wymiarów d')
plt.ylabel('Wartość wskaźnika R(d)')
plt.title('Koncentracja odległości')
plt.yscale('log')
plt.grid(True)
plt.show()