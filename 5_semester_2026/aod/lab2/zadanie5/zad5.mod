set Districts;
set Shifts;

param minCars{Districts, Shifts}, integer, >= 0;
param maxCars{d in Districts, s in Shifts}, integer, >= minCars[d, s];
param shiftMin{Shifts}, integer, >= 0;
param districtMin{Districts}, integer, >= 0;

var x{d in Districts, s in Shifts}, integer, >= minCars[d, s], <= maxCars[d, s];

minimize TotalCars:
    sum{d in Districts, s in Shifts} x[d, s];

s.t. ShiftRequirement{s in Shifts}:
    sum{d in Districts} x[d, s] >= shiftMin[s];

s.t. DistrictRequirement{d in Districts}:
    sum{s in Shifts} x[d, s] >= districtMin[d];

solve;

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
