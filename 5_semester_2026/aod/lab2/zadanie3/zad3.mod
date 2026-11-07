/* Zadanie 3 - plan produkcji i magazynowania (LP)
   Autor: Jakub Retmańczyk */

/* ---------- Parametry ---------- */
param K, integer, >= 1;              # liczba okresów
set Periods := 1..K;                 # okresy planowania

param normalCap, >= 0;               # limit produkcji normalnej [szt./okres]
param storageCap, >= 0;              # pojemność magazynu [szt.]
param storageCost, >= 0;             # koszt przechowania [$/szt./okres]
param initialStock, >= 0;            # początkowy stan magazynu [szt.]

param c{Periods}, >= 0;              # koszt produkcji normalnej [$/szt.]
param a{Periods}, >= 0;              # limit produkcji ponadwymiarowej [szt.]
param o{Periods}, >= 0;              # koszt produkcji ponadwymiarowej [$/szt.]
param d{Periods}, >= 0;              # zapotrzebowanie [szt.]

/* ---------- Zmienne decyzyjne ---------- */
var x{j in Periods}, >= 0, <= normalCap;   # produkcja normalna w okresie j [szt.]
var y{j in Periods}, >= 0, <= a[j];        # produkcja ponadwymiarowa w okresie j [szt.]
var s{j in Periods}, >= 0, <= storageCap;  # stan magazynu na koniec okresu j [szt.]

/* ---------- Funkcja celu ---------- */
minimize TotalCost:                  # łączny koszt produkcji i magazynowania [$]
    sum{j in Periods} (c[j] * x[j] + o[j] * y[j] + storageCost * s[j]);

/* ---------- Ograniczenia ---------- */
/* Bilans magazynu: stan poprzedni + produkcja - popyt = stan bieżący. */
s.t. Balance{j in Periods}:
    (if j = 1 then initialStock else s[j - 1]) + x[j] + y[j] - d[j] = s[j];

solve;

/* ---------- Raport ---------- */
printf "Minimalny koszt: %.2f $\n", TotalCost;
printf "okres  normalna  ponadwym.  magazyn  popyt\n";
for {j in Periods}
    printf "%5d  %8.2f  %9.2f  %7.2f  %5g\n", j, x[j], y[j], s[j], d[j];
printf "Produkcja ponadwymiarowa w okresach:";
for {j in Periods: y[j] > 1e-6} printf " %d", j;
printf "\nMagazyn wypełniony w okresach:";
for {j in Periods: s[j] >= storageCap - 1e-6} printf " %d", j;
printf "\n";
end;
