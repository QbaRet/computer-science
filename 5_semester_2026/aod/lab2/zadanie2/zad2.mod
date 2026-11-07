/* Zadanie 2 - tygodniowy plan produkcji (LP)
   Autor: Jakub Retmańczyk */

/* ---------- Zbiory ---------- */
set Products;                        # wyroby
set Machines;                        # maszyny

/* ---------- Parametry ---------- */
param price{Products}, >= 0;         # cena sprzedaży [$/kg]
param material{Products}, >= 0;      # koszt materiałowy [$/kg]
param maxDemand{Products}, >= 0;     # maksymalny tygodniowy popyt [kg]
param machineCost{Machines}, >= 0;   # koszt pracy maszyny [$/h]
param hours{Machines}, >= 0;         # tygodniowa dostępność maszyny [h]
param time{Products, Machines}, >= 0;   # czas obróbki [min/kg]

/* Zysk jednostkowy: cena - materiał - koszt czasu pracy maszyn [$/kg]. */
param unitProfit{p in Products} :=
    price[p] - material[p] - sum{m in Machines} time[p, m] / 60 * machineCost[m];

/* ---------- Zmienne decyzyjne ---------- */
var x{p in Products}, >= 0, <= maxDemand[p];  # tygodniowa produkcja [kg]; górne ograniczenie to popyt

/* ---------- Funkcja celu ---------- */
maximize Profit:                     # tygodniowy zysk [$]
    sum{p in Products} unitProfit[p] * x[p];

/* ---------- Ograniczenia ---------- */
/* Łączny czas pracy maszyny nie przekracza jej dostępności (w minutach). */
s.t. MachineTime{m in Machines}:
    sum{p in Products} time[p, m] * x[p] <= 60 * hours[m];

solve;

/* ---------- Raport ---------- */
printf "Maksymalny zysk: %.4f $\n", Profit;
printf "Plan produkcji [kg] (zysk jednostkowy [$/kg]):\n";
for {p in Products}
    printf "  %s: %.4f / %g (%.4f)\n", p, x[p], maxDemand[p], unitProfit[p];
printf "Wykorzystanie maszyn [min]:\n";
for {m in Machines}
    printf "  %s: %.2f / %g\n", m, sum{p in Products} time[p, m] * x[p], 60 * hours[m];
end;
