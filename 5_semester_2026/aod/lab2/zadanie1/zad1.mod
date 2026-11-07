/* Zadanie 1 - zakup i dostawy paliwa (problem transportowy, LP)
   Autor: Jakub Retmańczyk */

/* ---------- Zbiory ---------- */
set Suppliers;                       # dostawcy paliwa (firmy)
set Airports;                        # lotniska

/* ---------- Parametry ---------- */
param supply{Suppliers}, >= 0;       # maksymalna podaż firmy [galony]
param demand{Airports}, >= 0;        # zapotrzebowanie lotniska [galony]
param cost{Airports, Suppliers}, >= 0;  # koszt dostawy jednego galonu [$/galon]

/* Warunek konieczny istnienia rozwiązania dopuszczalnego. */
check: sum{s in Suppliers} supply[s] >= sum{a in Airports} demand[a];

/* ---------- Zmienne decyzyjne ---------- */
var x{Suppliers, Airports}, >= 0;    # ilość paliwa z firmy s na lotnisko a [galony]

/* ---------- Funkcja celu ---------- */
minimize TotalCost:                  # łączny koszt zakupu i dostaw [$]
    sum{s in Suppliers, a in Airports} cost[a, s] * x[s, a];

/* ---------- Ograniczenia ---------- */
/* Firma nie może dostarczyć więcej, niż wynosi jej podaż. */
s.t. SupplyLimit{s in Suppliers}:
    sum{a in Airports} x[s, a] <= supply[s];

/* Każde lotnisko musi otrzymać dokładnie tyle paliwa, ile potrzebuje. */
s.t. DemandMet{a in Airports}:
    sum{s in Suppliers} x[s, a] = demand[a];

solve;

/* ---------- Raport ---------- */
printf "Minimalny koszt: %.2f $\n", TotalCost;
printf "Plan dostaw [galony]:\n";
for {s in Suppliers, a in Airports: x[s, a] > 1e-6}
    printf "  %s -> %s: %.2f\n", s, a, x[s, a];
printf "Wykorzystanie podaży:\n";
for {s in Suppliers}
    printf "  %s: %.2f / %.2f (zapas %.2f)\n", s,
        sum{a in Airports} x[s, a], supply[s],
        supply[s] - sum{a in Airports} x[s, a];
end;
