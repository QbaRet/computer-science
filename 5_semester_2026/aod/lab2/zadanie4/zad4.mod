/* Zadanie 4 - najtańsza ścieżka z ograniczeniem czasu (MIP)
   Autor: Jakub Retmańczyk

   Relaksację liniową uzyskuje się bez zmiany modelu, przez przełącznik
   glpsol --nomip (zmienne pozostają w przedziale [0, 1]).
   Wersję bez ograniczenia czasu uzyskuje się przez useTime := 0 w danych. */

/* ---------- Zbiory ---------- */
set Nodes;                           # miasta
set Arcs within Nodes cross Nodes;   # połączenia skierowane

/* ---------- Parametry ---------- */
param source symbolic, in Nodes;     # miasto początkowe i°
param target symbolic, in Nodes;     # miasto docelowe j°
param T, >= 0;                       # limit łącznego czasu przejazdu
param useTime, binary, default 1;    # 1 = ograniczenie czasu aktywne
param c{Arcs}, >= 0;                 # koszt przejazdu łukiem
param t{Arcs}, >= 0;                 # czas przejazdu łukiem

/* ---------- Zmienne decyzyjne ---------- */
var x{Arcs}, binary;                 # 1, gdy łuk należy do ścieżki

/* ---------- Funkcja celu ---------- */
minimize PathCost:                   # łączny koszt ścieżki
    sum{(i, j) in Arcs} c[i, j] * x[i, j];

/* ---------- Ograniczenia ---------- */
/* Zachowanie przepływu: jedna jednostka wypływa z source i wpływa do target. */
s.t. FlowBalance{v in Nodes}:
    sum{(v, j) in Arcs} x[v, j] - sum{(i, v) in Arcs} x[i, v]
    = (if v = source then 1 else if v = target then -1 else 0);

/* Łączny czas przejazdu nie przekracza T (gdy useTime = 1). */
s.t. TimeLimit{dummy in 1..1: useTime = 1}:
    sum{(i, j) in Arcs} t[i, j] * x[i, j] <= T;

solve;

/* ---------- Raport ---------- */
printf "Koszt: %.4f\n", PathCost;
printf "Czas:  %.4f (limit %g, aktywny: %d)\n",
    sum{(i, j) in Arcs} t[i, j] * x[i, j], T, useTime;
printf "Niezerowe zmienne x:\n";
for {(i, j) in Arcs: x[i, j] > 1e-6}
    printf "  %s -> %s: %.4f (koszt %g, czas %g)\n", i, j, x[i, j], c[i, j], t[i, j];
end;
