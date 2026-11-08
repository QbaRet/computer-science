set Suppliers;
set Airports;

param supply{Suppliers}, >= 0;
param demand{Airports}, >= 0;
param cost{Airports, Suppliers}, >= 0;

check: sum{s in Suppliers} supply[s] >= sum{a in Airports} demand[a];

var x{Suppliers, Airports}, >= 0;

minimize TotalCost:
    sum{s in Suppliers, a in Airports} cost[a, s] * x[s, a];

s.t. SupplyLimit{s in Suppliers}:
    sum{a in Airports} x[s, a] <= supply[s];

s.t. DemandMet{a in Airports}:
    sum{s in Suppliers} x[s, a] = demand[a];

solve;

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
