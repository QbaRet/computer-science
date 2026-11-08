param K, integer, >= 1;
set Periods := 1..K;

param normalCap, >= 0;
param storageCap, >= 0;
param storageCost, >= 0;
param initialStock, >= 0;

param c{Periods}, >= 0;
param a{Periods}, >= 0;
param o{Periods}, >= 0;
param d{Periods}, >= 0;

var x{j in Periods}, >= 0, <= normalCap;
var y{j in Periods}, >= 0, <= a[j];
var s{j in Periods}, >= 0, <= storageCap;

minimize TotalCost:
    sum{j in Periods} (c[j] * x[j] + o[j] * y[j] + storageCost * s[j]);

s.t. Balance{j in Periods}:
    (if j = 1 then initialStock else s[j - 1]) + x[j] + y[j] - d[j] = s[j];

solve;

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
