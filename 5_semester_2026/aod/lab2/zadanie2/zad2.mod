set Products;
set Machines;

param price{Products}, >= 0;
param material{Products}, >= 0;
param maxDemand{Products}, >= 0;
param machineCost{Machines}, >= 0;
param hours{Machines}, >= 0;
param time{Products, Machines}, >= 0;

param unitProfit{p in Products} :=
    price[p] - material[p] - sum{m in Machines} time[p, m] / 60 * machineCost[m];

var x{p in Products}, >= 0, <= maxDemand[p];

maximize Profit:
    sum{p in Products} unitProfit[p] * x[p];

s.t. MachineTime{m in Machines}:
    sum{p in Products} time[p, m] * x[p] <= 60 * hours[m];

solve;

printf "Maksymalny zysk: %.4f $\n", Profit;
printf "Plan produkcji [kg] (zysk jednostkowy [$/kg]):\n";
for {p in Products}
    printf "  %s: %.4f / %g (%.4f)\n", p, x[p], maxDemand[p], unitProfit[p];
printf "Wykorzystanie maszyn [min]:\n";
for {m in Machines}
    printf "  %s: %.2f / %g\n", m, sum{p in Products} time[p, m] * x[p], 60 * hours[m];
end;
