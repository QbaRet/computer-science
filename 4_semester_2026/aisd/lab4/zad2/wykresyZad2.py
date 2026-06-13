#!/usr/bin/env python3

import csv
import sys

import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


def load_csv(path="wyniki2.csv"):
    rows = []
    with open(path, newline="") as file_handle:
        reader = csv.DictReader(file_handle)
        for row in reader:
            rows.append(
                {
                    "n": int(row["N"]),
                    "scenario": int(row["Scenario"]),
                    "operation": row["Operation"],
                    "avg_cmp": float(row["AvgCmp"]),
                    "avg_ptr": float(row["AvgPtr"]),
                    "avg_height": float(row["AvgHeight"]),
                }
            )

    rows.sort(key=lambda item: (item["n"], item["scenario"], item["operation"]))
    return rows


def build_series(rows, metric_name):
    series = {
        (1, "Insert"): ([], []),
        (1, "Delete"): ([], []),
        (2, "Insert"): ([], []),
        (2, "Delete"): ([], []),
    }

    for row in rows:
        key = (row["scenario"], row["operation"])
        series[key][0].append(row["n"])
        series[key][1].append(row[metric_name])

    return series


def plot_metric(ax, rows, metric_name, title, ylabel):
    series = build_series(rows, metric_name)
    styles = {
        (1, "Insert"): {"label": "Scenariusz 1 - Insert", "color": "#1f77b4", "marker": "o"},
        (1, "Delete"): {"label": "Scenariusz 1 - Delete", "color": "#ff7f0e", "marker": "^"},
        (2, "Insert"): {"label": "Scenariusz 2 - Insert", "color": "#2ca02c", "marker": "s"},
        (2, "Delete"): {"label": "Scenariusz 2 - Delete", "color": "#d62728", "marker": "D"},
    }

    for key, style in styles.items():
        ns, values = series[key]
        ax.plot(
            ns,
            values,
            linewidth=2,
            markersize=4,
            marker=style["marker"],
            color=style["color"],
            label=style["label"],
        )

    ax.set_title(title, fontsize=13)
    ax.set_ylabel(ylabel, fontsize=12)
    ax.grid(True, linestyle="--", alpha=0.5)
    ax.legend(fontsize=10)
    ax.xaxis.set_major_locator(ticker.MaxNLocator(nbins=10, integer=True))


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "wyniki2.csv"

    try:
        rows = load_csv(path)
    except FileNotFoundError:
        if path == "wyniki2.csv":
            rows = load_csv("wyniki.csv")
            path = "wyniki.csv"
        else:
            raise

    print(f"Wczytano dane z: {path}")

    fig, axes = plt.subplots(3, 2, figsize=(16, 14), sharex=True)

    plot_metric(axes[0][0], rows, "avg_cmp", "Średnia liczba porównań", "AvgCmp")
    plot_metric(axes[0][1], rows, "max_cmp", "Maksymalna liczba porównań", "MaxCmp")
    plot_metric(
        axes[1][0],
        rows,
        "avg_ptr",
        "Średnia liczba operacji na wskaźnikach",
        "AvgPtr",
    )
    plot_metric(
        axes[1][1],
        rows,
        "max_ptr",
        "Maksymalna liczba operacji na wskaźnikach",
        "MaxPtr",
    )
    plot_metric(axes[2][0], rows, "avg_height", "Średnia wysokość drzewa", "AvgHeight")
    plot_metric(axes[2][1], rows, "max_height", "Maksymalna wysokość drzewa", "MaxHeight")

    axes[2][0].set_xlabel("Liczba elementów N", fontsize=12)
    axes[2][1].set_xlabel("Liczba elementów N", fontsize=12)
    fig.suptitle("Zadanie 2 - charakterystyka drzewa BST", fontsize=15)
    plt.tight_layout(rect=(0, 0, 1, 0.97))

    output_path = "zad2_wykresy.png"
    plt.savefig(output_path, dpi=150)
    print(f"Wykres zapisany: {output_path}")
    plt.show()


if __name__ == "__main__":
    main()