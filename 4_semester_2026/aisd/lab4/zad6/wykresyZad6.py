#!/usr/bin/env python3

import csv
import math
import sys

import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


def load_csv(path="wyniki_zad6.csv"):
    ns = []
    srednie = []
    minima = []
    maxima = []

    with open(path, newline="") as file_handle:
        reader = csv.DictReader(file_handle)
        for row in reader:
            ns.append(int(row["n"]))
            srednie.append(float(row["srednia_rund"]))
            minima.append(float(row["min_rund"]))
            maxima.append(float(row["max_rund"]))

    return ns, srednie, minima, maxima


def plot_stats(ns, srednie, minima, maxima):
    fig, ax = plt.subplots(figsize=(10, 6))

    ax.plot(
        ns,
        srednie,
        marker="o",
        linewidth=2,
        markersize=4,
        label="Średnia liczba rund",
        color="#1f77b4",
    )
    ax.plot(
        ns,
        minima,
        marker="^",
        linewidth=1.8,
        markersize=4,
        label="Minimum",
        color="#2ca02c",
    )
    ax.plot(
        ns,
        maxima,
        marker="s",
        linewidth=1.8,
        markersize=4,
        label="Maksimum",
        color="#d62728",
    )
    ax.fill_between(ns, minima, maxima, color="#1f77b4", alpha=0.12, label="Zakres min-max")

    ax.set_xlabel("Liczba wierzchołków n", fontsize=13)
    ax.set_ylabel("Liczba rund", fontsize=13)
    ax.set_title("Rozsyłanie wiadomości w MST - statystyki rund", fontsize=14)
    ax.legend(fontsize=11)
    ax.grid(True, linestyle="--", alpha=0.5)
    ax.xaxis.set_major_locator(ticker.MaxNLocator(nbins=10, integer=True))

    plt.tight_layout()
    output_path = "zad6_rundy.png"
    plt.savefig(output_path, dpi=150)
    print(f"Wykres 1 zapisany: {output_path}")


def plot_normalized(ns, srednie, minima, maxima):
    fig, ax = plt.subplots(figsize=(10, 6))

    srednie_norm = [srednie[i] / math.log2(ns[i]) for i in range(len(ns))]
    minima_norm = [minima[i] / math.log2(ns[i]) for i in range(len(ns))]
    maxima_norm = [maxima[i] / math.log2(ns[i]) for i in range(len(ns))]

    ax.plot(
        ns,
        srednie_norm,
        marker="o",
        linewidth=2,
        markersize=4,
        label="Średnia / log2(n)",
        color="#1f77b4",
    )
    ax.plot(
        ns,
        minima_norm,
        marker="^",
        linewidth=1.8,
        markersize=4,
        label="Minimum / log2(n)",
        color="#2ca02c",
    )
    ax.plot(
        ns,
        maxima_norm,
        marker="s",
        linewidth=1.8,
        markersize=4,
        label="Maksimum / log2(n)",
        color="#d62728",
    )

    ax.set_xlabel("Liczba wierzchołków n", fontsize=13)
    ax.set_ylabel("Rundy / log2(n)", fontsize=13)
    ax.set_title("Znormalizowany wykres liczby rund", fontsize=14)
    ax.legend(fontsize=11)
    ax.grid(True, linestyle="--", alpha=0.5)
    ax.xaxis.set_major_locator(ticker.MaxNLocator(nbins=10, integer=True))

    plt.tight_layout()
    output_path = "zad6_rundy_normalized.png"
    plt.savefig(output_path, dpi=150)
    print(f"Wykres 2 zapisany: {output_path}")


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "wyniki_zad6.csv"

    try:
        ns, srednie, minima, maxima = load_csv(path)
    except FileNotFoundError:
        if path == "wyniki_zad6.csv":
            ns, srednie, minima, maxima = load_csv("wyniki.csv")
            path = "wyniki.csv"
        else:
            raise

    print(f"Wczytano dane z: {path}")
    plot_stats(ns, srednie, minima, maxima)
    plot_normalized(ns, srednie, minima, maxima)
    plt.show()


if __name__ == "__main__":
    main()