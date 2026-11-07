/* Zadanie 5 - przydział radiowozów do dzielnic i zmian (ILP)
   Autor: Jakub Retmańczyk */

/* ---------- Zbiory ---------- */
set Districts;                       # dzielnice
set Shifts;                          # zmiany

/* ---------- Parametry ---------- */
param minCars{Districts, Shifts}, integer, >= 0;  # dolny limit dla pary [radiowozy]
param maxCars{d in Districts, s in Shifts}, integer, >= minCars[d, s];  # górny limit [radiowozy]
param shiftMin{Shifts}, integer, >= 0;      # minimum na zmianę [radiowozy]
param districtMin{Districts}, integer, >= 0;  # minimum dla dzielnicy [radiowozy]

/* ---------- Zmienne decyzyjne ---------- */
/* Liczba radiowozów w dzielnicy d na zmianie s; granice to Tabele 1 i 2. */
var x{d in Districts, s in Shifts}, integer, >= minCars[d, s], <= maxCars[d, s];

/* ---------- Funkcja celu ---------- */
minimize TotalCars:                  # całkowita liczba radiowozów
    sum{d in Districts, s in Shifts} x[d, s];

/* ---------- Ograniczenia ---------- */
/* Na każdej zmianie dostępne jest co najmniej shiftMin radiowozów. */
s.t. ShiftRequirement{s in Shifts}:
    sum{d in Districts} x[d, s] >= shiftMin[s];

/* Każda dzielnica ma przypisane co najmniej districtMin radiowozów. */
s.t. DistrictRequirement{d in Districts}:
    sum{s in Shifts} x[d, s] >= districtMin[d];

solve;

/* ---------- Raport ---------- */
printf "Całkowita liczba radiowozów: %d\n", TotalCars;
printf "%-8s", "";
for {s in Shifts} printf "%6s", s;
printf "  razem (min)\n";
for {d in Districts} {
    printf "%-8s", d;
    for {s in Shifts} printf "%6d", x[d, s];
    printf "  %5d (%d)\n", sum{s in Shifts} x[d, s], districtMin[d];
}
printf "%-8s", "razem";
for {s in Shifts} printf "%6d", sum{d in Districts} x[d, s];
printf "\n%-8s", "(min)";
for {s in Shifts} printf "%6d", shiftMin[s];
printf "\n";
end;
