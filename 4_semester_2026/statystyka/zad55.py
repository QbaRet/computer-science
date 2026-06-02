import numpy as np
import matplotlib.pyplot as plt

X=np.random.normal(0,1,100)
e=np.random.normal(0,0.25,100)
Y=-1+0.5*X+e
print("Theta0:", -1, "Theta1:", 0.5)
print("Dlugosc wektora Y:", len(Y))


plt.scatter(X,Y)
plt.xlabel("X")
plt.ylabel("Y")
plt.title("Wykres rozrzutu")
plt.show()


