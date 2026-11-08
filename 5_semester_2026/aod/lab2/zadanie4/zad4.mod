set Nodes;
set Arcs within Nodes cross Nodes;

param source symbolic, in Nodes;
param target symbolic, in Nodes;
param T, >= 0;
param useTime, binary, default 1;
param c{Arcs}, >= 0;
param t{Arcs}, >= 0;

var x{Arcs}, binary;

minimize PathCost:
    sum{(i, j) in Arcs} c[i, j] * x[i, j];

s.t. FlowBalance{v in Nodes}:
    sum{(v, j) in Arcs} x[v, j] - sum{(i, v) in Arcs} x[i, v]
    = (if v = source then 1 else if v = target then -1 else 0);

s.t. TimeLimit{dummy in 1..1: useTime = 1}:
    sum{(i, j) in Arcs} t[i, j] * x[i, j] <= T;

solve;

printf "Koszt: %.4f\n", PathCost;
printf "Czas:  %.4f (limit %g, aktywny: %d)\n",
    sum{(i, j) in Arcs} t[i, j] * x[i, j], T, useTime;
printf "Niezerowe zmienne x:\n";
for {(i, j) in Arcs: x[i, j] > 1e-6}
    printf "  %s -> %s: %.4f (koszt %g, czas %g)\n", i, j, x[i, j], c[i, j], t[i, j];
end;
