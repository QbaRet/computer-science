import numpy as np
import matplotlib.pyplot as plt

lambd0 = 2
lambd_range = np.linspace(0.5, 4, 1000)
n_values = [5, 30, 200]
B = 1000 

n_30 = 30
wynik = np.random.exponential(1/lambd0, n_30)

def log_likelihood(l, data):
    n = len(data)
    return n * np.log(l) - l * np.sum(data)

mle_30 = 1 / np.mean(wynik)


plt.figure(figsize=(12, 5))
plt.subplot(1, 2, 1)
ll_vals = [log_likelihood(l, wynik) for l in lambd_range]
plt.plot(lambd_range, ll_vals, label=f'n={n_30}')
plt.scatter(mle_30, log_likelihood(mle_30, wynik), color='red', zorder=5)
plt.title('Punkt (b): Log-wiarygodność dla n=30')
plt.xlabel('$\lambda$')
plt.ylabel('$\ell(\lambda)$')
plt.legend()

plt.subplot(1, 2, 2)
for n in n_values:
    sample = np.random.exponential(1/lambd0, n)
    ll_norm = np.log(lambd_range) - lambd_range * np.mean(sample)
    plt.plot(lambd_range, ll_norm, label=f'n={n}')

plt.axvline(lambd0, color='black', linestyle='--', alpha=0.5, label='Prawdziwe $\lambda_0$')
plt.title('Punkt (c): Porównanie $\ell(\lambda)/n$')
plt.xlabel('$\lambda$')
plt.ylabel('$\ell(\lambda)/n$')
plt.legend()
plt.tight_layout()
plt.show()

print(f"Punkt (d): Badanie obciążenia (B={B} powtórzeń)")
print(f"{'n':>5} | {'Średnia z daszkiem':>20} | {'Obciążenie':>12}")
print("-" * 45)

for n in n_values:
    mle_list = []
    for _ in range(B):
        sample = np.random.exponential(1/lambd0, n)
        mle_list.append(1 / np.mean(sample))
    
    avg_mle = np.mean(mle_list)
    bias = avg_mle - lambd0
    print(f"{n:5d} | {avg_mle:20.4f} | {bias:12.4f}")