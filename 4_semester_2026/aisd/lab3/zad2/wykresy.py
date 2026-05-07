from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


ALGORITHMS = ["SELECT", "RAND_SELECT"]
K_LABELS = {
    1: "min",
    2: "0.25n",
    3: "0.5n",
    4: "0.75n",
    5: "max",
}
COLORS = {
    "SELECT": "#1f77b4",
    "RAND_SELECT": "#d62728",
}


def load_data(csv_path: Path) -> pd.DataFrame:
    df = pd.read_csv(csv_path)
    expected = {"n", "k_typ", "algorytm", "avg_porownania", "avg_przestawienia"}
    missing = expected - set(df.columns)
    if missing:
        raise ValueError(f"Brakuje kolumn w CSV: {', '.join(sorted(missing))}")

    df = df.copy()
    df["n"] = pd.to_numeric(df["n"])
    df["k_typ"] = pd.to_numeric(df["k_typ"])
    df["avg_porownania"] = pd.to_numeric(df["avg_porownania"])
    df["avg_przestawienia"] = pd.to_numeric(df["avg_przestawienia"])
    return df


def plot_metric(df: pd.DataFrame, metric: str, ylabel: str, output_path: Path) -> None:
    fig, axes = plt.subplots(5, 1, figsize=(13, 18), sharex=True)
    fig.suptitle(f"{ylabel} dla SELECT i RAND_SELECT", fontsize=18)

    for idx, k_typ in enumerate([1, 2, 3, 4, 5]):
        ax = axes[idx]
        subset_k = df[df["k_typ"] == k_typ]

        for algo in ALGORITHMS:
            subset = subset_k[subset_k["algorytm"] == algo].sort_values("n")
            if subset.empty:
                continue

            ax.plot(
                subset["n"],
                subset[metric],
                marker="o",
                markersize=3,
                linewidth=1.5,
                label=algo,
                color=COLORS.get(algo, "black"),
            )

        ax.set_title(f"k_typ = {k_typ} ({K_LABELS.get(k_typ, '?')})")
        ax.set_ylabel(ylabel)
        ax.grid(True, alpha=0.3)
        ax.legend(loc="upper left")

    axes[-1].set_xlabel("n")
    plt.tight_layout(rect=[0, 0.02, 1, 0.97])
    fig.savefig(output_path, dpi=200)
    plt.close(fig)


def plot_ratio(df: pd.DataFrame, output_path: Path) -> None:
    ratio_df = df.copy()
    ratio_df["porownania_na_n"] = ratio_df["avg_porownania"] / ratio_df["n"]
    ratio_df["przestawienia_na_n"] = ratio_df["avg_przestawienia"] / ratio_df["n"]

    fig, axes = plt.subplots(5, 2, figsize=(16, 18), sharex=True)
    fig.suptitle("Koszt jednostkowy względem n", fontsize=18)

    for idx, k_typ in enumerate([1, 2, 3, 4, 5]):
        subset_k = ratio_df[ratio_df["k_typ"] == k_typ]

        for col_idx, (metric, ylabel) in enumerate(
            [("porownania_na_n", "avg_porownania / n"), ("przestawienia_na_n", "avg_przestawienia / n")]
        ):
            ax = axes[idx, col_idx]
            for algo in ALGORITHMS:
                subset = subset_k[subset_k["algorytm"] == algo].sort_values("n")
                if subset.empty:
                    continue
                ax.plot(
                    subset["n"],
                    subset[metric],
                    marker="o",
                    markersize=3,
                    linewidth=1.5,
                    label=algo,
                    color=COLORS.get(algo, "black"),
                )

            if idx == 0:
                ax.set_title(ylabel)
            ax.set_ylabel(f"k={k_typ}")
            ax.grid(True, alpha=0.3)
            ax.legend(loc="upper left")

    for ax in axes[-1, :]:
        ax.set_xlabel("n")

    plt.tight_layout(rect=[0, 0.02, 1, 0.97])
    fig.savefig(output_path, dpi=200)
    plt.close(fig)


def main() -> None:
    parser = argparse.ArgumentParser(description="Rysowanie wykresów z wyniki.csv")
    parser.add_argument("csv", nargs="?", default="wyniki.csv", help="Ścieżka do pliku CSV")
    parser.add_argument("-o", "--output-dir", default=".", help="Katalog docelowy dla PNG")
    args = parser.parse_args()

    csv_path = Path(args.csv)
    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    df = load_data(csv_path)
    df = df.sort_values(["k_typ", "algorytm", "n"])

    plot_metric(df, "avg_porownania", "Średnia liczba porównań", output_dir / "wykres_porownania.png")
    plot_metric(df, "avg_przestawienia", "Średnia liczba przestawień", output_dir / "wykres_przestawienia.png")
    plot_ratio(df, output_dir / "wykres_koszt_jednostkowy.png")

    print(f"Zapisano wykresy do: {output_dir.resolve()}")


if __name__ == "__main__":
    main()
