#!/usr/bin/env python3

import csv
import math
import sys

import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


def load_csv(path="wyniki.csv"):
        ns, prim_matrix, prim_heap, kruskals = [], [], [], []
        with open(path) as f:
                reader = csv.DictReader(f)
                for row in reader:
                        ns.append(int(row["n"]))
                        if "prim_macierz_us" in row:
                                prim_matrix.append(float(row["prim_macierz_us"]))
                        else:
                                prim_matrix.append(float(row["prim_us"]))
                        if "prim_kopiec_us" in row:
                                prim_heap.append(float(row["prim_kopiec_us"]))
                        else:
                                prim_heap.append(float(row["prim_us"]))
                        kruskals.append(float(row["kruskal_us"]))
        return ns, prim_matrix, prim_heap, kruskals


def main():
        path = sys.argv[1] if len(sys.argv) > 1 else "wyniki.csv"
        try:
                ns, prim_matrix, prim_heap, kruskals = load_csv(path)
        except FileNotFoundError:
                if path == "results.csv":
                        ns, prim_matrix, prim_heap, kruskals = load_csv("wyniki.csv")
                        path = "wyniki.csv"
                else:
                        raise

        fig1, ax1 = plt.subplots(figsize=(10, 6))

        ax1.plot(
                ns,
                prim_matrix,
                marker="o",
                linewidth=2,
                markersize=4,
                label="Prim macierzowy",
                color="#2196F3",
        )
        ax1.plot(
                ns,
                prim_heap,
                marker="^",
                linewidth=2,
                markersize=4,
                label="Prim kopcowy",
                color="#FF9800",
        )
        ax1.plot(
                ns,
                kruskals,
                marker="s",
                linewidth=2,
                markersize=4,
                label="Kruskal",
                color="#F44336",
        )

        ax1.set_xlabel("Liczba wierzchołków  n", fontsize=13)
        ax1.set_ylabel("Średni czas [μs]", fontsize=13)
        ax1.set_title(
                "Porównanie czasów algorytmów\n(pełny graf ważony, wagi ∈ (0,1))",
                fontsize=14,
        )
        ax1.legend(fontsize=12)
        ax1.grid(True, linestyle="--", alpha=0.5)
        ax1.xaxis.set_major_locator(ticker.MaxNLocator(nbins=10, integer=True))

        plt.tight_layout()
        out1 = "mst_benchmark_plot3.png"
        plt.savefig(out1, dpi=150)
        print(f"Wykres 1 (bezwzględny) zapisany: {out1}")

        fig2, ax2 = plt.subplots(figsize=(10, 6))

        prim_matrix_normalized = [prim_matrix[i] / (ns[i] ** 2) for i in range(len(ns))]
        prim_heap_normalized = [
                prim_heap[i] / (ns[i] ** 2 * math.log2(ns[i])) for i in range(len(ns))
        ]
        kruskal_normalized = [
                kruskals[i] / (ns[i] ** 2 * math.log2(ns[i])) for i in range(len(ns))
        ]

        ax2.plot(
                ns,
                prim_matrix_normalized,
                marker="o",
                linewidth=2,
                markersize=4,
                label="Prim macierzowy  T(n) / n²",
                color="#2196F3",
        )
        ax2.plot(
                ns,
                prim_heap_normalized,
                marker="^",
                linewidth=2,
                markersize=4,
                label="Prim kopcowy  T(n) / (n² log n)",
                color="#FF9800",
        )
        ax2.plot(
                ns,
                kruskal_normalized,
                marker="s",
                linewidth=2,
                markersize=4,
                label="Kruskal  T(n) / (n² log n)",
                color="#F44336",
        )

        ax2.set_xlabel("Liczba wierzchołków  n", fontsize=13)
        ax2.set_ylabel("Znormalizowany czas [μs]", fontsize=13)
        ax2.set_title(
                "Weryfikacja złożoności po normalizacji\n(podział przez n² oraz n² log n)",
                fontsize=14,
        )
        ax2.legend(fontsize=12)
        ax2.grid(True, linestyle="--", alpha=0.5)
        ax2.xaxis.set_major_locator(ticker.MaxNLocator(nbins=10, integer=True))

        plt.tight_layout()
        out2 = "mst_benchmark_normalized.png"
        plt.savefig(out2, dpi=150)
        print(f"Wykres 2 (znormalizowany) zapisany: {out2}")

        plt.show()


if __name__ == "__main__":
        main()
 