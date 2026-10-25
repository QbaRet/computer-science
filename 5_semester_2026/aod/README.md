# Laboratorium 1 - Algorytmy Optymalizacji Dyskretnej

Repozytorium zawiera implementację zadań z Listy 1 (Podstawowe algorytmy grafowe).

---

## Struktura projektu

```
.
├── Makefile
├── aod_laby1.pdf
├── sciaga.md                   <- kompendium wiedzy i ściągawka do obrony
└── lab1/
    ├── zadanie1/
    │   ├── zad1.cpp            <- DFS i BFS
    │   ├── *.txt               <- grafy testowe (Rysunek 1 + własne)
    │   └── raport.md           <- tabela wyników
    ├── zadanie2/
    │   ├── zad2.cpp            <- Sortowanie topologiczne (algorytm Kahna)
    │   ├── dag.txt, cykl.txt
    │   └── raport.md           <- tabela wyników
    ├── zadanie3/
    │   ├── zad3.cpp            <- Silnie spójne składowe (algorytm Kosaraju)
    │   ├── silniespojny.txt, spojneskladowe.txt
    │   └── raport.md           <- tabela wyników
    └── zadanie4/
        ├── zad4.cpp            <- Badanie dwudzielności grafu (2-kolorowanie BFS)
        ├── dwu_D.txt, dwu_U.txt, ndwu_D.txt, ndwu_U.txt
        └── raport.md           <- tabela wyników
```

---

## Kompilacja

```bash
make
```

---

## Uruchamianie

Każdy program czyta graf ze standardowego wejścia `std::cin` lub bezpośrednio z pliku:

```bash
# Zadanie 1:
cd lab1/zadanie1
./zad1 < g1_skierowany.txt
./zad1 --tree < g1_skierowany.txt

# Zadanie 2:
cd ../zadanie2
./zad2 < dag.txt
./zad2 < cykl.txt

# Zadanie 3:
cd ../zadanie3
./zad3 < silniespojny.txt
./zad3 < spojneskladowe.txt

# Zadanie 4:
cd ../zadanie4
./zad4 < dwu_D.txt
./zad4 < ndwu_D.txt
```
