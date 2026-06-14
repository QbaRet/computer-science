import argparse
import csv
import os
import sys
 
import matplotlib.pyplot as plt


COUNTRY_TO_TSP = {
    "Uruguay": "uy734.tsp",
    "Canada": "ca4663.tsp",
    "Oman": "mu1979.tsp",
    "Tanzania": "tz6117.tsp",
    "Egypt": "eg7146.tsp",
    "Ireland": "ei8246.tsp",
    "Sahara": "wi29.tsp",
    "Qatar": "qa194.tsp",
    "Djibouti": "dj38.tsp",
    "Zimbabwe": "zi929.tsp",
}


def parse_route_points(route_text):
    points = []
    if not route_text:
        return points

    for chunk in route_text.split("|"):
        chunk = chunk.strip()
        if not chunk:
            continue

        parts = chunk.split(":")
        if len(parts) != 3:
            continue

        try:
            city_id = int(parts[0])
            x = float(parts[1])
            y = float(parts[2])
        except ValueError:
            continue

        points.append((city_id, x, y))

    return points


def load_results(csv_path):
    rows = []

    try:
        csv.field_size_limit(sys.maxsize)
    except OverflowError:
        csv.field_size_limit(int(sys.maxsize // 2))

    with open(csv_path, newline="", encoding="utf-8") as csv_file:
        reader = csv.DictReader(csv_file)
        for row in reader:
            try:
                row["cost"] = float(row["cost"])
                row["cities"] = int(row["cities"])
                row["simulation_id"] = int(row["simulation_id"])
                row["steps"] = int(row["steps"])
            except (KeyError, TypeError, ValueError):
                continue

            row["route"] = parse_route_points(row.get("route_points", ""))
            if row["route"]:
                rows.append(row)

    return rows


def load_tsp_coords(tsp_path):
    coords = {}
    in_section = False

    with open(tsp_path, encoding="utf-8") as tsp_file:
        for raw_line in tsp_file:
            line = raw_line.strip()
            if not line:
                continue

            if line == "NODE_COORD_SECTION":
                in_section = True
                continue

            if line == "EOF":
                break

            if not in_section:
                continue

            parts = line.split()
            if len(parts) < 3:
                continue

            try:
                node_id = int(parts[0])
                x = float(parts[1])
                y = float(parts[2])
            except ValueError:
                continue

            coords[node_id] = (x, y)

    return coords


def resolve_tsp_path(country, tsp_dir):
    tsp_name = COUNTRY_TO_TSP.get(country)
    if tsp_name is None:
        return None
    return os.path.join(tsp_dir, tsp_name)


def select_best_rows_by_country(rows):
    best_rows = {}

    for row in rows:
        country = row["country"]
        current_best = best_rows.get(country)
        if current_best is None or row["cost"] < current_best["cost"]:
            best_rows[country] = row

    return best_rows


def plot_best_route(row, tsp_coords, output_dir):
    route_ids = [city_id for city_id, _, _ in row["route"]]
    route_coords = []

    for city_id in route_ids:
        coord = tsp_coords.get(city_id)
        if coord is None:
            coord = next((pair[1:] for pair in row["route"] if pair[0] == city_id), None)
        if coord is not None:
            route_coords.append(coord)

    if not route_coords:
        return None

    if route_coords[0] != route_coords[-1]:
        route_coords = route_coords + [route_coords[0]]

    xs = [point[0] for point in route_coords]
    ys = [point[1] for point in route_coords]

    all_xs = [coord[0] for coord in tsp_coords.values()] if tsp_coords else xs
    all_ys = [coord[1] for coord in tsp_coords.values()] if tsp_coords else ys
    plt.figure(figsize=(10, 10))
    plt.scatter(all_xs, all_ys, color="#c7c7c7", s=2, alpha=0.6, zorder=1)
    plt.plot(xs, ys, color="royalblue", linewidth=1.2, zorder=2)
    plt.scatter(xs[:-1], ys[:-1], color="red", s=4, zorder=3)
    plt.scatter(xs[0], ys[0], color="limegreen", s=40, marker="o", label="Start", zorder=4)
    plt.scatter(xs[-2], ys[-2], color="darkorange", s=40, marker="s", label="Koniec", zorder=4)

    plt.title(
        f"Najlepsza trasa: {row['country']} | sim {row['simulation_id']} | cost {row['cost']:.2f}",
        fontsize=13,
        fontweight="bold",
    )
    plt.xlabel("Wspolrzedna X")
    plt.ylabel("Wspolrzedna Y")
    plt.grid(True, linestyle="--", alpha=0.35)
    plt.legend()
    plt.tight_layout()

    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(
        output_dir,
        f"najlepsza_trasa_{row['country'].lower()}_sim{row['simulation_id']}.png",
    )
    plt.savefig(output_path, dpi=300)
    plt.close()
    return output_path


def main():
    parser = argparse.ArgumentParser(
        description="Rysuje wykresy najlepszych tras z pliku wyniki_ga2.csv."
    )
    parser.add_argument(
        "csv_path",
        nargs="?",
        default="wyniki_ga2.csv",
        help="Sciezka do pliku wyniki_ga2.csv",
    )
    parser.add_argument(
        "--tsp-dir",
        default=".",
        help="Katalog z plikami .tsp",
    )
    parser.add_argument(
        "--output-dir",
        default="wykresy_best_route",
        help="Katalog docelowy na obrazy",
    )

    args = parser.parse_args()

    rows = load_results(args.csv_path)
    best_rows = select_best_rows_by_country(rows)

    if not best_rows:
        print("Brak danych do narysowania.")
        return

    for country in sorted(best_rows):
        best_row = best_rows[country]
        tsp_path = resolve_tsp_path(country, args.tsp_dir)

        if tsp_path is None or not os.path.exists(tsp_path):
            print(f"Brak pliku TSP dla kraju {country}.")
            continue

        tsp_coords = load_tsp_coords(tsp_path)
        output_path = plot_best_route(best_row, tsp_coords, args.output_dir)

        if output_path is None:
            print(f"Nie udalo sie zbudowac trasy dla {country}.")
            continue

        print(f"Wygenerowano: {output_path}")


if __name__ == "__main__":
    main()