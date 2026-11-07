/* Zadanie 6 - rozmieszczenie kamer na terenie składowym (ILP)
   Autor: Jakub Retmańczyk

   Założenie: kontenery nie zasłaniają widoku kamery, bo treść tego nie wymaga. */

/* ---------- Parametry ---------- */
param m, integer, >= 1;              # liczba wierszy terenu
param n, integer, >= 1;              # liczba kolumn terenu
param k, integer, >= 1;              # zasięg kamery [kwadraty]

set Rows := 1..m;
set Cols := 1..n;
set Containers within Rows cross Cols;   # kwadraty zajęte przez kontenery

/* Kwadraty wolne, czyli dopuszczalne miejsca dla kamer. */
set Free := (Rows cross Cols) diff Containers;

/* ---------- Zmienne decyzyjne ---------- */
var y{Free}, binary;                 # 1, gdy w kwadracie stoi kamera

/* ---------- Funkcja celu ---------- */
minimize Cameras:                    # liczba użytych kamer
    sum{(i, j) in Free} y[i, j];

/* ---------- Ograniczenia ---------- */
/* Każdy kontener widzi co najmniej jedna kamera z tego samego wiersza lub
   tej samej kolumny, odległa o co najwyżej k kwadratów. */
s.t. Covered{(i, j) in Containers}:
    sum{(p, q) in Free: (p = i and abs(q - j) <= k)
                     or (q = j and abs(p - i) <= k)} y[p, q] >= 1;

solve;

/* ---------- Raport ---------- */
printf "k = %d, liczba kamer: %d\n", k, Cameras;
printf "Legenda: K = kontener, C = kamera, . = wolne\n";
for {i in Rows} {
    for {j in Cols}
        printf "%s ", if (i, j) in Containers then "K"
                      else if y[i, j] > 0.5 then "C" else ".";
    printf "\n";
}
end;
