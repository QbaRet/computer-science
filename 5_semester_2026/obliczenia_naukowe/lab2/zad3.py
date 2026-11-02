# Autor: Jakub Retmańczyk
# Lista 2, zadanie 3 - drugi program do wizualizacji (matplotlib)
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

x = np.arange(0, 60, 0.05)
naiwnie = np.exp(x) * np.log(1 + np.exp(-x))   # naiwny wzór
stabilnie = np.exp(x) * np.log1p(np.exp(-x))   # log1p
plt.plot(x, naiwnie, label="naiwnie")
plt.plot(x, stabilnie, "--", label="log1p")
plt.axhline(1, color="k", ls=":", label="granica = 1")
plt.xlabel("x"); plt.ylabel("f(x)"); plt.legend()
plt.title("f(x) = e^x ln(1+e^{-x})")
plt.savefig("zad3_python.png", dpi=150)
