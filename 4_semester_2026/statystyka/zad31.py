import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm, t


def confidence_interval(mu, sigma, n, alpha=0.05, rng=None):
    if rng is None:
        rng = np.random.default_rng()

    sample = rng.normal(mu, sigma, n)
    x_bar = np.mean(sample)
    s = np.std(sample, ddof=1)

    t_quantile = t.ppf(1 - alpha / 2, df=n - 1)
    margin = t_quantile * s / np.sqrt(n)
    ci = (x_bar - margin, x_bar + margin)
    covers_mu = ci[0] <= mu <= ci[1]

    return ci, covers_mu


def simulate_t_intervals(mu, sigma, n, reps=1000, alpha=0.05, rng=None):
    if rng is None:
        rng = np.random.default_rng()

    samples = rng.normal(mu, sigma, size=(reps, n))
    means = samples.mean(axis=1)
    s_vals = samples.std(axis=1, ddof=1)

    t_quantile = t.ppf(1 - alpha / 2, df=n - 1)
    margins = t_quantile * s_vals / np.sqrt(n)

    lower = means - margins
    upper = means + margins
    widths = upper - lower
    covers = (lower <= mu) & (mu <= upper)

    return lower, upper, covers, widths


def known_sigma_width(sigma, n, alpha=0.05):
    z_quantile = norm.ppf(1 - alpha / 2)
    return 2 * z_quantile * sigma / np.sqrt(n)


def task_a(mu=260, sigma=18, n=9, reps=1000, alpha=0.05, rng=None):
    lower, upper, covers, widths = simulate_t_intervals(mu, sigma, n, reps, alpha, rng)
    coverage = covers.mean()
    avg_width = widths.mean()
    width_known = known_sigma_width(sigma, n, alpha)

    print("(a) n =", n)
    print(f"Empiryczna czestosc pokrycia: {coverage:.4f} ({coverage * 100:.2f}%)")
    print(f"Srednia szerokosc CI (sigma nieznane): {avg_width:.4f}")
    print(f"Szerokosc CI (sigma znane):            {width_known:.4f}")
    print()


def task_b(mu=260, sigma=18, n=9, reps=100, alpha=0.05, rng=None):
    lower, upper, covers, _ = simulate_t_intervals(mu, sigma, n, reps, alpha, rng)

    y = np.arange(1, reps + 1)
    colors = np.where(covers, "tab:blue", "tab:red")

    plt.figure(figsize=(10, 8))
    for i in range(reps):
        plt.hlines(y[i], lower[i], upper[i], color=colors[i], linewidth=2)

    plt.axvline(mu, color="black", linestyle="--", linewidth=1.5, label=f"mu = {mu}")
    plt.xlabel("Wartosc mu")
    plt.ylabel("Numer przedzialu")
    plt.title(f"{reps} przedzialow ufnosci 95% (n={n})")
    plt.legend()
    plt.tight_layout()
    plt.show()

    red_percent = (~covers).mean() * 100
    print("(b)")
    print(f"Procent odcinkow czerwonych (niepokrywajacych mu): {red_percent:.2f}%")
    print()


def task_c(mu=260, sigma=18, n_values=(5, 9, 30, 100), reps=1000, alpha=0.05, rng=None):
    print("(c)")
    for n in n_values:
        lower, upper, covers, widths = simulate_t_intervals(mu, sigma, n, reps, alpha, rng)
        coverage = covers.mean()
        avg_width = widths.mean()
        print(
            f"n={n:>3} | pokrycie: {coverage:.4f} ({coverage * 100:.2f}%) | "
            f"srednia szerokosc: {avg_width:.4f}"
        )


if __name__ == "__main__":
    rng = np.random.default_rng(2026)
    ci, covers = confidence_interval(mu=260, sigma=18, n=9, rng=rng)
    print("Pojedynczy przedzial:", ci, "| pokrywa mu:", covers)
    print()

    task_a(mu=260, sigma=18, n=9, reps=1000, rng=rng)
    task_b(mu=260, sigma=18, n=9, reps=100, rng=rng)
    task_c(mu=260, sigma=18, n_values=(5, 9, 30, 100), reps=1000, rng=rng)