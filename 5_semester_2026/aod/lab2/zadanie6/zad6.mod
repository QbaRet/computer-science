param m, integer, >= 1;
param n, integer, >= 1;
param k, integer, >= 1;

set Rows := 1..m;
set Cols := 1..n;
set Containers within Rows cross Cols;

set Free := (Rows cross Cols) diff Containers;

var y{Free}, binary;

minimize Cameras:
    sum{(i, j) in Free} y[i, j];

s.t. Covered{(i, j) in Containers}:
    sum{(p, q) in Free: (p = i and abs(q - j) <= k)
                     or (q = j and abs(p - i) <= k)} y[p, q] >= 1;

solve;

printf "k = %d, liczba kamer: %d\n", k, Cameras;
printf "Legenda: K = kontener, C = kamera, . = wolne\n";
for {i in Rows} {
    for {j in Cols}
        printf "%s ", if (i, j) in Containers then "K"
                      else if y[i, j] > 0.5 then "C" else ".";
    printf "\n";
}
end;
